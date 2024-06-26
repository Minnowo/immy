#include "../imylib2.h"

#ifdef BUILD_JPEG_LOADER 
#include "../imlib2/loaders/loader_jpeg.c"

ImlibLoadStatus_t il2LoadJPEG(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}
#endif

