#include "../imylib2.h"
#include "../imlib2/loaders/loader_ico.c"

ImlibLoadStatus_t il2LoadICO(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}

