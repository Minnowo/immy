

#include <pthread.h>
#include <raylib.h>
#include <unistd.h>

#include "doko.h"
#include "config.h"
#include "external/hashmap.h"

typedef struct {

        pthread_t       thread;
        pthread_mutex_t mutex;
        bool            finished;
        char*           path;
        Image           rayim;

} doko_thread_t;

typedef struct {
        const doko_image_t* key;
        doko_thread_t*      value;
} doko_hashmap_thread_t;


// for hashmap key comparison
int _thread_cmp(const void* a, const void* b, void* udata) {
    const doko_hashmap_thread_t* at = a;
    const doko_hashmap_thread_t* bt = b;
    return at->key - bt->key;
}

// for hashmap hashing of key
uint64_t _thread_hash(const void* item, uint64_t seed0, uint64_t seed1) {
    const doko_hashmap_thread_t* THREAD = item;
    return hashmap_sip(THREAD->key, sizeof(THREAD->key), seed0, seed1);
}

// maps our doko_image_t* to the thread loading said image
struct hashmap* thread_map;

// to make stuff a little bit nicer to read
#define HM_DELETE(x) hashmap_delete(thread_map, &(doko_hashmap_thread_t){.key = (x)})
#define HM_GET(x) hashmap_get(thread_map, &(doko_hashmap_thread_t){.key = (x)})
#define HM_PUT(x, y) hashmap_get(thread_map, &(doko_hashmap_thread_t){.key = (x), .value=(y)})

// annoyingly this hashmap can only be init using heap
// so we can't just have it defined above
static inline void hashmap_init() {

    if(thread_map == NULL) {

        // this should only ever be run once
        thread_map = hashmap_new(
            sizeof(doko_hashmap_thread_t), 0, 0, 0, _thread_hash, _thread_cmp,
            NULL, NULL
        );

        if(thread_map == NULL) {
            DIE("Cannot alloc a new hashmap");
        }
    }
}

// deletes the item from the hashmap and frees stuff
static void doko_async_destroy_thread(const doko_image_t* im, bool image_gotten) {

    // // this is not needed
    // hashmap_init();

    L_D("%s: Removing item from thread hashmap", __func__);

    const doko_hashmap_thread_t* THREAD = HM_DELETE(im);

    if (!image_gotten)
        UnloadImage(THREAD->value->rayim);

    if (THREAD != NULL)
        if (THREAD->value != NULL) {
            pthread_mutex_destroy(&THREAD->value->mutex);
            free(THREAD->value->path);
            free(THREAD->value);
        }
}


bool doko_async_has_image(const doko_image_t* im) {

    hashmap_init();

    return HM_GET(im) != NULL;
}


bool doko_async_get_image(doko_image_t* im) {

    const doko_hashmap_thread_t* ITEM = HM_GET(im);

    if(ITEM == NULL) { 
        return false;
    }

    doko_thread_t* thread = ITEM->value;

    pthread_mutex_lock(&thread->mutex);

    int finished = thread->finished;

    pthread_mutex_unlock(&thread->mutex);

    if(!finished) {
        return false;
    }

    // thread is gone
    L_D("%s: Async image load finished", __func__);

    if (!IsImageReady(thread->rayim)) {

        im->status = IMAGE_STATUS_FAILED;

        L_W("%s: Loaded image was invalid for %s", im->path);

        goto done;
    }

    im->rayim = thread->rayim;
    im->srcRect = (Rectangle){
        0.0,
        0.0,
        im->rayim.width,
        im->rayim.height,
    };
    im->dstPos = (Vector2){0, 0};
    im->status = IMAGE_STATUS_LOADED;

done:
    doko_async_destroy_thread(im, true);

    return true;
}

void *async_image_load_thread_main(void * raw_arg) {

    L_D("%s: Thread is running", __func__);

    doko_thread_t* thread = raw_arg;

    L_D("%s: Thread is about to load %s", __func__, thread->path);

    // imlib2 is not thread-safe, we cannot use it here
    //
    if (
#ifdef DOKO_USE_MAGICK
        !(doko_load_with_magick_stdout(thread->path, &thread->rayim)) &&
#endif

#ifdef DOKO_USE_FFMPEG
        !(doko_load_with_ffmpeg_stdout(thread->path, &thread->rayim)) &&
#endif
        true) {

        thread->rayim = LoadImage(thread->path);
    }

    L_D("%s: Thread is done", __func__);

    pthread_mutex_lock(&thread->mutex);
    thread->finished = true;
    pthread_mutex_unlock(&thread->mutex);

    return NULL;
}

bool doko_async_load_image(const doko_image_t* im) {

    if (doko_async_has_image(im))
        return false;

    doko_thread_t* thread = malloc(sizeof(doko_thread_t));

    if (thread == NULL)
        return false;

    thread->finished = false;
    thread->path = doko_strdup(im->path);

    if (thread->path == NULL) {

        free(thread);

        return false;
    }

    pthread_mutex_init(&thread->mutex, NULL);

    if(pthread_create(&thread->thread, NULL, async_image_load_thread_main, (void*)thread) != 0) {

        free(thread->path);
        free(thread);
        return false;
    }

    doko_hashmap_thread_t item = {.key = im, .value = thread};

    hashmap_set(thread_map, &item);

    return true;
}


