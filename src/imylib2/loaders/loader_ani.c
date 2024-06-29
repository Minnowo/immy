#include "../imylib2.h"

#ifdef __unix__
#include "../imlib2/loaders/loader_ani.c"

ImlibLoadStatus_t il2LoadANI(struct ImlibImage *im, int load_data) {
    return _load((ImlibImage*)im, load_data);
}
#endif

