#include "../imylib2.h"

#ifdef BUILD_J2K_LOADER 
#include "../imlib2/loaders/loader_j2k.c"

ImlibLoadStatus_t il2LoadJ2K(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}
#endif

