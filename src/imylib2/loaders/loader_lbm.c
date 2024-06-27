#include "../imylib2.h"
#include "../imlib2/loaders/loader_lbm.c"

ImlibLoadStatus_t il2LoadLBM(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}

