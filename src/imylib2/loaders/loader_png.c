#include "../imylib2.h"

#if defined(BUILD_PNG_LOADER) && defined(__unix__) 
#include "../imlib2/loaders/loader_png.c"

ImlibLoadStatus_t il2LoadPNG(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

