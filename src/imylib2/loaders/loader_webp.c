#include "../imylib2.h"

#ifdef BUILD_WEBP_LOADER 
#include "../imlib2/loaders/loader_webp.c"

ImlibLoadStatus_t il2LoadWEBP(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

