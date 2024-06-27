#include "../imylib2.h"

#ifdef BUILD_BZ2_LOADER 
#include "../imlib2/loaders/loader_bz2.c"

ImlibLoadStatus_t il2LoadBZ2(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

