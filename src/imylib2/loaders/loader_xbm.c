#include "../imylib2.h"
#include "../imlib2/loaders/loader_xbm.c"

ImlibLoadStatus_t il2LoadXBM(struct ImlibImage  *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}

