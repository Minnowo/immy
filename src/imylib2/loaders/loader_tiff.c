#include "../imylib2.h"

#ifdef BUILD_TIFF_LOADER 
#include "../imlib2/loaders/loader_tiff.c"

ImlibLoadStatus_t il2LoadTIFF(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

