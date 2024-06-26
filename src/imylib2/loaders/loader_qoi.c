#include "../imylib2.h"
#include "../imlib2/loaders/loader_qoi.c"

ImlibLoadStatus_t il2LoadQOI(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}

