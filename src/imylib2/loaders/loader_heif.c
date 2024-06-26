#include "../imylib2.h"

#ifdef BUILD_HEIF_LOADER
#include "../imlib2/loaders/loader_heif.c"

ImlibLoadStatus_t il2LoadHEIF(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}
#endif

