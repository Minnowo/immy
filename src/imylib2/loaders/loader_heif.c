#include "../imylib2.h"

#ifdef BUILD_HEIF_LOADER
#include "../imlib2/loaders/loader_heif.c"

ImlibLoadStatus_t il2LoadHEIF(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

