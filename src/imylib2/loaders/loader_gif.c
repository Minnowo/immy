#include "../imylib2.h"

#ifdef BUILD_GIF_LOADER
#include "../imlib2/loaders/loader_gif.c"

ImlibLoadStatus_t il2LoadGIF(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

