#include "../imylib2.h"
#include "../imlib2/loaders/loader_bmp.c"

ImlibLoadStatus_t il2LoadBMP(ImlibImage *im, int load_data) {
    return _load(im, load_data);
}

