
#ifndef IMYLIB2_H
#define IMYLIB2_H

// #include "./imlib2/loaders/config.h"
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

typedef struct {
    int             left, right, top, bottom;
} ImlibBorder;

typedef void   *(*ImlibImageDataMemoryFunction)(void *, size_t size);

struct ImlibImage {
    ImlibImageFileInfo *fi;
    ImlibLoaderCtx *lc;

    int             w, h;
    uint32_t       *data;
    char            has_alpha;
    char            rsvd[3];

    int             frame;

    /* vvv Private vvv */
    ImlibLoader    *loader;
    ImlibImage     *next;

    char           *file;
    char           *key;
    uint64_t        moddate;
    unsigned int    flags;
    int             references;
    char           *format;

    ImlibBorder     border;
    ImlibImageTag  *tags;
    ImlibImageDataMemoryFunction data_memory_func;

    ImlibImageFrame *pframe;
    /* ^^^ Private ^^^ */
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


typedef ImlibLoadStatus_t (*il2Loader)(struct ImlibImage *im, int load_data);


int il2DefaultProgress(ImlibImage * im, char percent, int update_x, int update_y, int update_w, int update_h);

bool il2FileContextOpen(ImlibImageFileInfo* fi);
bool il2FileContextOpenEx(ImlibImageFileInfo* fi, FILE* fp, const void* fdata, off_t fsize);
void il2FileContextClose(ImlibImageFileInfo* fi);

bool il2LoadImageAsBGRA(const char* path, struct ImlibImage* image);
bool il2LoadImageAsRGBA(const char* path, struct ImlibImage* image);

ImlibLoadStatus_t il2LoadQOI(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadBMP(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadARGB(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadICO(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadLBM(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadPNM(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadTGA(struct ImlibImage *im, int load_data);
ImlibLoadStatus_t il2LoadFF(struct ImlibImage *im, int load_data);
// ImlibLoadStatus_t il2LoadXPM(struct ImlibImage *im, int load_data);


#ifdef __unix__
ImlibLoadStatus_t il2LoadXBM(struct ImlibImage *im, int load_data);
#endif

// #ifdef BUILD_BZ2_LOADER 
// ImlibLoadStatus_t il2LoadBZ2(struct ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_GIF_LOADER
// ImlibLoadStatus_t il2LoadGIF(struct ImlibImage *im, int load_data);
// #endif

#ifdef BUILD_HEIF_LOADER
ImlibLoadStatus_t il2LoadHEIF(struct ImlibImage *im, int load_data);
#endif

// #ifdef BUILD_ID3_LOADER
// ImlibLoadStatus_t il2LoadID3(struct ImlibImage *im, int load_data);
// #endif

#ifdef BUILD_J2K_LOADER
ImlibLoadStatus_t il2LoadJ2K(struct ImlibImage *im, int load_data);
#endif

#ifdef BUILD_JPEG_LOADER
ImlibLoadStatus_t il2LoadJPEG(struct ImlibImage *im, int load_data);
#endif

#ifdef BUILD_JXL_LOADER
ImlibLoadStatus_t il2LoadJXL(struct ImlibImage *im, int load_data);
#endif

// #ifdef BUILD_LZMA_LOADER
// ImlibLoadStatus_t il2LoadLZMA(struct ImlibImage *im, int load_data);
// #endif

#ifdef BUILD_PNG_LOADER
ImlibLoadStatus_t il2LoadPNG(struct ImlibImage *im, int load_data);
#endif

// #ifdef BUILD_PS_LOADER
// ImlibLoadStatus_t il2LoadPS(struct ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_RAW_LOADER
// ImlibLoadStatus_t il2LoadRAW(struct ImlibImage *im, int load_data);
// #endif

// #ifdef BUILD_SVG_LOADER
// ImlibLoadStatus_t il2LoadSVG(struct ImlibImage *im, int load_data);
// #endif

#ifdef BUILD_TIFF_LOADER
ImlibLoadStatus_t il2LoadTIFF(struct ImlibImage *im, int load_data);
#endif

#ifdef BUILD_WEBP_LOADER
ImlibLoadStatus_t il2LoadWEBP(struct ImlibImage *im, int load_data);
#endif




#endif
