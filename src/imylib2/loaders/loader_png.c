#include "../imylib2.h"

#ifdef BUILD_PNG_LOADER 
#include "../imlib2/loaders/loader_png.c"

ImlibLoadStatus_t il2LoadPNG(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

