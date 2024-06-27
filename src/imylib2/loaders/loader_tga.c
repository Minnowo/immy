#include "../imylib2.h"
#include "../imlib2/loaders/loader_tga.c"

ImlibLoadStatus_t il2LoadTGA(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}

