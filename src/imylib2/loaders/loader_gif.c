#include "../imylib2.h"

#ifdef BUILD_GIF_LOADER
#include "../imlib2/loaders/loader_gif.c"

ImlibLoadStatus_t il2LoadGIF(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}
#endif

