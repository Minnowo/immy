
#ifndef IMMY_IMLIB2_WRAPPER_H
#define IMMY_IMLIB2_WRAPPER_H

#include "./imlib2/loaders/config.h"
#include "./imlib2/Imlib2_Loader.h"

#include <stdbool.h>

#undef IMLIB_LOADER
#define IMLIB_LOADER(x,y,z)

typedef int (*ImlibProgressFunction)(
    ImlibImage* im, char percent, int update_x, int update_y, int update_w, int update_h
);

typedef struct {
        FILE*                 fp;
        const void*           fdata;
        size_t                fsize;
        ImlibProgressFunction pfunc;
        int                   pgran;
        char                  immed;
        char                  nocache;
        int                   err;
        int                   frame;
} ImlibLoadArgs;

struct _ImlibLoaderCtx {
        ImlibProgressFunction progress;
        char                  granularity;
        int                   pct;
        int                   area;
        int                   row;
        int                   pass;
        int                   n_pass;
};

#define LOAD_BREAK 2     /* Break signaled by progress callback */
#define LOAD_SUCCESS 1   /* Image loaded successfully           */
#define LOAD_FAIL 0      /* Image was not recognized by loader  */
#define LOAD_OOM -1      /* Could not allocate memory           */
#define LOAD_BADFILE -2  /* File could not be accessed          */
#define LOAD_BADIMAGE -3 /* Image is corrupt                    */
#define LOAD_BADFRAME -4 /* Requested frame not found           */

typedef enum {
    IMLIB_STATUS_LOAD_BREAK    = LOAD_BREAK,
    IMLIB_STATUS_LOAD_SUCCESS  = LOAD_SUCCESS,
    IMLIB_STATUS_LOAD_FAIL     = LOAD_FAIL,
    IMLIB_STATUS_LOAD_OOM      = LOAD_OOM,
    IMLIB_STATUS_LOAD_BADFILE  = LOAD_BADFILE,
    IMLIB_STATUS_LOAD_BADIMAGE = LOAD_BADIMAGE,
    IMLIB_STATUS_LOAD_BADFRAME = LOAD_BADFRAME
} ImlibLoadStatus_t;



int il2DefaultProgress(ImlibImage * im, char percent, int update_x, int update_y, int update_w, int update_h);

bool il2FileContextOpen(ImlibImageFileInfo* fi);
bool il2FileContextOpenEx(ImlibImageFileInfo* fi, FILE* fp, const void* fdata, off_t fsize);
void il2FileContextClose(ImlibImageFileInfo* fi);

ImlibLoadStatus_t il2LoadQOI(ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadBMP(ImlibImage *im, int load_data);

// #ifdef BUILD_BZ2_LOADER 
// ImlibLoadStatus_t il2LoadBZ2(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_GIF_LOADER
// ImlibLoadStatus_t il2LoadGIF(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_HEIF_LOADER
// ImlibLoadStatus_t il2LoadHEIF(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_ID3_LOADER
// ImlibLoadStatus_t il2LoadID3(ImlibImage *im, int load_data);
// #endif

#ifdef BUILD_J2K_LOADER
ImlibLoadStatus_t il2LoadJ2K(ImlibImage *im, int load_data);
#endif

#ifdef BUILD_JPEG_LOADER
ImlibLoadStatus_t il2LoadJPEG(ImlibImage *im, int load_data);
#endif

// #ifdef BUILD_JXL_LOADER
// ImlibLoadStatus_t il2LoadJXL(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_LZMA_LOADER
// ImlibLoadStatus_t il2LoadLZMA(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_PNG_LOADER
// ImlibLoadStatus_t il2LoadPNG(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_PS_LOADER
// ImlibLoadStatus_t il2LoadPS(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_RAW_LOADER
// ImlibLoadStatus_t il2LoadRAW(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_SVG_LOADER
// ImlibLoadStatus_t il2LoadSVG(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_TIFF_LOADER
// ImlibLoadStatus_t il2LoadTIFF(ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_WEBP_LOADER
// ImlibLoadStatus_t il2LoadWEBP(ImlibImage *im, int load_data);
// #endif




#endif
