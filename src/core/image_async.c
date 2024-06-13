
#include <pthread.h>
#include <raylib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../external/hashmap.h"
#include "core.h"

typedef struct {

        pthread_t       thread;
        pthread_mutex_t mutex;
        bool            finished;
        bool            dothumbnail;
        char*           path;
        immy_image_t    im;
} immy_thread_t;

typedef struct {
        const immy_image_t* key;
        immy_thread_t*      value;
} immy_hashmap_thread_t;


// for hashmap key comparison
int _thread_cmp(const void* a, const void* b, void* udata) {
    const immy_hashmap_thread_t* at = a;
    const immy_hashmap_thread_t* bt = b;
    return at->key - bt->key;
}

// for hashmap hashing of key
uint64_t _thread_hash(const void* item, uint64_t seed0, uint64_t seed1) {
    const immy_hashmap_thread_t* THREAD = item;
    return hashmap_sip(THREAD->key, sizeof(THREAD->key), seed0, seed1);
}

// maps our immy_image_t* to the thread loading said image
struct hashmap* thread_map;

// to make stuff a little bit nicer to read
#define HM_DELETE(x) hashmap_delete(thread_map, &(immy_hashmap_thread_t){.key = (x)})
#define HM_GET(x) hashmap_get(thread_map, &(immy_hashmap_thread_t){.key = (x)})
#define HM_PUT(x, y) hashmap_set(thread_map, &(immy_hashmap_thread_t){.key = (x), .value=(y)})

// annoyingly this hashmap can only be init using heap
// so we can't just have it defined above
static inline void hashmap_init() {

    if(thread_map == NULL) {

        // this should only ever be run once
        thread_map = hashmap_new(
            sizeof(immy_hashmap_thread_t), 0, 0, 0, _thread_hash, _thread_cmp,
            NULL, NULL
        );

        if(thread_map == NULL) {
            DIE("Cannot alloc a new hashmap");
        }
    }
}

// deletes the item from the hashmap and frees stuff
static void immy_async_destroy_thread(const immy_image_t* im, bool image_gotten) {

    hashmap_init();

    L_D("%s: Removing item from thread hashmap", __func__);

    const immy_hashmap_thread_t* THREAD = HM_DELETE(im);

    if (!THREAD)
        return;

    if (!image_gotten) {
        UnloadImage(THREAD->value->im.rayim);
        UnloadImage(THREAD->value->im.thumb);
    }

    if (THREAD->value != NULL) {
        pthread_mutex_destroy(&THREAD->value->mutex);
        free(THREAD->value->path);
        free(THREAD->value);
    }
}


bool immy_async_has_image(const immy_image_t* im) {

    hashmap_init();

    return HM_GET(im) != NULL;
}


bool immy_async_get_image(immy_image_t* im) {

    const immy_hashmap_thread_t* ITEM = HM_GET(im);

    if(ITEM == NULL) { 
        return false;
    }

    immy_thread_t* thread = ITEM->value;

    pthread_mutex_lock(&thread->mutex);

    bool finished = thread->finished;
    bool didThumb = thread->dothumbnail;

    pthread_mutex_unlock(&thread->mutex);

    if(!finished) {
        return false;
    }

    // thread is gone
    L_D("%s: Async image load finished", __func__);

    if (!IsImageReady(thread->im.rayim)) {

        im->status = IMAGE_STATUS_FAILED;

        L_W("%s: Loaded image was invalid for %s", im->path);

        goto done;
    }

    im->rayim = thread->im.rayim;
    im->srcRect = (Rectangle){
        0.0,
        0.0,
        im->rayim.width,
        im->rayim.height,
    };
    im->dstPos = (Vector2){0, 0};
    im->status = IMAGE_STATUS_LOADED;

    // reset thumbnail status so we can maybe load it now
    if(im->thumb_status == IMAGE_STATUS_FAILED)
        im->thumb_status = IMAGE_STATUS_NOT_LOADED;

#if GENERATE_THUMB_WHEN_LOADING_IMAGE

    if (didThumb) {

        if (IsImageReady(thread->im.thumb)) {

            UnloadImage(im->thumb);

            im->thumb = thread->im.thumb;
            im->thumb_status = IMAGE_STATUS_LOADED;

        } else if (!IsImageReady(im->thumb)) {

            im->thumb_status = IMAGE_STATUS_NOT_LOADED;

            memset(&im->thumb, 0, sizeof(im->thumb));
        }
    }
#endif

done:
    immy_async_destroy_thread(im, true);

    return true;
}

void *async_image_load_thread_main(void * raw_arg) {

    L_D("%s: Thread is running", __func__);

    immy_thread_t* thread = raw_arg;

    L_D("%s: Thread is about to load %s", __func__, thread->path);

    // imlib2 is not thread-safe, we cannot use it here
    //
    if (
#ifdef IMMY_USE_MAGICK
        !(immy_load_with_magick_stdout(thread->path, &thread->im.rayim)) &&
#endif

#ifdef IMMY_USE_FFMPEG
        !(immy_load_with_ffmpeg_stdout(thread->path, &thread->im.rayim)) &&
#endif
        true) {

        thread->im.rayim = LoadImage(thread->path);
    }


#if GENERATE_THUMB_WHEN_LOADING_IMAGE

    if (thread->dothumbnail && IsImageReady(thread->im.rayim)) {

        thread->im.status = IMAGE_STATUS_LOADED;

        if (!immyGetOrCreateThumb(&thread->im)) 

            L_W("%s: Could not create thumbnail", __func__);
    }

#endif

    pthread_mutex_lock(&thread->mutex);
    thread->finished = true;
    pthread_mutex_unlock(&thread->mutex);

    L_D("%s: Thread is done", __func__);

    return NULL;
}

bool immy_async_load_image(const immy_image_t* im) {

    if (immy_async_has_image(im))
        return false;

    immy_thread_t* thread = calloc(1, sizeof(immy_thread_t));

    if (thread == NULL)
        return false;

    thread->finished    = false;
    thread->path        = immyStrdup(im->path);
    thread->im.path     = thread->path; // so we can use immyGetOrCreateThumb
    thread->dothumbnail = im->thumb_status != IMAGE_STATUS_LOADED;

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

    immy_hashmap_thread_t item = {.key = im, .value = thread};

    hashmap_set(thread_map, &item);

    return true;
}


bool immy_async_create_thumbnails(immy_control_t* ctrl) {


    return false;
}



