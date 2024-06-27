#include "../imylib2.h"

#ifdef BUILD_JXL_LOADER 
#include "../imlib2/loaders/loader_jxl.c"

ImlibLoadStatus_t il2LoadJXL(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

