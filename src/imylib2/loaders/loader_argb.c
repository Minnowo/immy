#include "../imylib2.h"
#include "../imlib2/loaders/loader_argb.c"

ImlibLoadStatus_t il2LoadARGB(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}

