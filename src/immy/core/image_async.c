
#include <pthread.h>
#include <raylib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../external/hashmap.h"
#include "core.h"

// must come after config.h
#if defined(IMYLIB2_AVAILABLE) && USE_IMYLIB2
#include <imylib2.h>
#endif

typedef struct {
        pthread_t       thread;
        pthread_mutex_t mutex;
        bool            finished;
        bool            dothumbnail;
        char*           path;
        ImmyImage_t    im;
} ImgLoadThreadData_t;

typedef struct {
        const ImmyImage_t*  key;
        ImgLoadThreadData_t* value;
} HashMapThreadData_t;

// maps our ImmyImage_t* to the thread loading said image
struct hashmap* asyncMap;

// to make stuff a little bit nicer to read
#define HM_DELETE(x) hashmap_delete(asyncMap, &(HashMapThreadData_t){.key = (x)})
#define HM_GET(x) hashmap_get(asyncMap, &(HashMapThreadData_t){.key = (x)})
#define HM_PUT(x, y) hashmap_set(asyncMap, &(HashMapThreadData_t){.key = (x), .value = (y)})

// for hashmap key comparison
int _thread_cmp(const void* a, const void* b, void* udata) {
    const HashMapThreadData_t* at = a;
    const HashMapThreadData_t* bt = b;
    return at->key - bt->key;
}

// for hashmap hashing of key
uint64_t _thread_hash(const void* item, uint64_t seed0, uint64_t seed1) {
    const HashMapThreadData_t* THREAD = item;
    return hashmap_sip(THREAD->key, sizeof(THREAD->key), seed0, seed1);
}

void* async_image_load_thread_main(void* raw_arg) {

    L_D("%s: Thread is running", __func__);

    ImgLoadThreadData_t* thread = raw_arg;

    L_D("%s: Thread is about to load %s", __func__, thread->path);

#ifdef IMYLIB2_H

    L_D("Using imylib2 to load image.");

    struct ImlibImage il2Image;

    if (il2LoadImageAsRGBA(thread->path, &il2Image)) {

        thread->im.rayim.data = il2Image.data;
        thread->im.rayim.width = il2Image.w;
        thread->im.rayim.height = il2Image.h;
        thread->im.rayim.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        thread->im.rayim.mipmaps = 1;
    }
    else

#endif

    // imlib2 is not thread-safe, we cannot use it here
    //
    if (
#ifdef IMMY_USE_MAGICK
        !(iLoadImageWithMagick(thread->path, &thread->im.rayim)) &&
#endif

#ifdef IMMY_USE_FFMPEG
        !(iLoadImageWithFFmpeg(thread->path, &thread->im.rayim)) &&
#endif
        true) {

        L_D("Using Raylib to load the image.");

        // raylibs load image checks file extension
        // so if it ends with .kra don't bother having raylib load it
        if (!iEndsWith(thread->path, ".kra", 1))
            thread->im.rayim = LoadImage(thread->path);

        if (!IsImageReady(thread->im.rayim))
            iLoadKritaImage(thread->path, &thread->im.rayim);
    }

#if GENERATE_THUMB_WHEN_LOADING_IMAGE

    if (thread->dothumbnail && IsImageReady(thread->im.rayim)) {

        thread->im.status = IMAGE_STATUS_LOADED;

        if (!iGetOrCreateThumb(&thread->im))

            L_W("%s: Could not create thumbnail", __func__);
    }

#endif

    pthread_mutex_lock(&thread->mutex);
    thread->finished = true;
    pthread_mutex_unlock(&thread->mutex);

    L_D("%s: Thread is done", __func__);

    return NULL;
}

// annoyingly this hashmap can only be init using heap
// so we can't just have it defined above
static inline void hashmap_init() {

    if (asyncMap == NULL) {

        // this should only ever be run once
        asyncMap = hashmap_new(sizeof(HashMapThreadData_t), 0, 0, 0, _thread_hash, _thread_cmp, NULL, NULL);

        if (asyncMap == NULL) {
            DIE("Cannot alloc a new hashmap");
        }
    }
}

// deletes the item from the hashmap and frees stuff
static void immy_async_destroy_thread(const ImmyImage_t* im, bool image_gotten) {

    hashmap_init();

    L_D("%s: Removing item from thread hashmap", __func__);

    const HashMapThreadData_t* THREAD = HM_DELETE(im);

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

bool iAsyncHasImage(const ImmyImage_t* im) {

    hashmap_init();

    return HM_GET(im) != NULL;
}

bool iGetImageAsync(ImmyImage_t* im) {

    const HashMapThreadData_t* ITEM = HM_GET(im);

    if (ITEM == NULL) {
        return false;
    }

    ImgLoadThreadData_t* thread = ITEM->value;

    pthread_mutex_lock(&thread->mutex);

    bool finished = thread->finished;
    bool didThumb = thread->dothumbnail;

    pthread_mutex_unlock(&thread->mutex);

    if (!finished) {
        return false;
    }

    // thread is gone
    L_D("%s: Async image load finished", __func__);

    if (!IsImageReady(thread->im.rayim)) {

        im->status = IMAGE_STATUS_FAILED;

        L_W("%s: Loaded image was invalid for %s", im->path);

        goto done;
    }

    im->rayim   = thread->im.rayim;
    im->srcRect = (Rectangle){
        0.0,
        0.0,
        im->rayim.width,
        im->rayim.height,
    };
    im->dstPos = (Vector2){0, 0};
    im->status = IMAGE_STATUS_LOADED;

    // reset thumbnail status so we can maybe load it now
    if (im->thumb_status == IMAGE_STATUS_FAILED)
        im->thumb_status = IMAGE_STATUS_NOT_LOADED;

#if GENERATE_THUMB_WHEN_LOADING_IMAGE

    if (didThumb) {

        if (IsImageReady(thread->im.thumb)) {

            UnloadImage(im->thumb);

            im->thumb        = thread->im.thumb;
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

bool iLoadImageAsync(const ImmyImage_t* im) {

    if (iAsyncHasImage(im))
        return false;

    ImgLoadThreadData_t* thread = calloc(1, sizeof(ImgLoadThreadData_t));

    if (thread == NULL)
        return false;

    thread->finished    = false;
    thread->path        = iStrDup(im->path);
    thread->im.path     = thread->path; // so we can use immyGetOrCreateThumb
    thread->dothumbnail = im->thumb_status != IMAGE_STATUS_LOADED;

    if (thread->path == NULL) {

        free(thread);

        return false;
    }

    pthread_mutex_init(&thread->mutex, NULL);

    if (pthread_create(&thread->thread, NULL, async_image_load_thread_main, (void*)thread) != 0) {

        free(thread->path);
        free(thread);
        return false;
    }

    HashMapThreadData_t item = {.key = im, .value = thread};

    hashmap_set(asyncMap, &item);

    return true;
}
