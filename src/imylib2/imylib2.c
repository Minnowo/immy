
#include "imylib2.h"

#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

il2Loader loaders[] = {

#ifdef BUILD_PNG_LOADER
    il2LoadPNG,
#endif

#ifdef BUILD_JPEG_LOADER
    il2LoadJPEG,
#endif

    il2LoadQOI,

#ifdef BUILD_WEBP_LOADER
    il2LoadWEBP,
#endif

#ifdef BUILD_GIF_LOADER
    il2LoadGIF,
#endif

#ifdef BUILD_JXL_LOADER
    il2LoadJXL,
#endif

    il2LoadBMP,

#ifdef BUILD_TIFF_LOADER
    il2LoadTIFF,
#endif

#ifdef BUILD_HEIF_LOADER
    il2LoadHEIF,
#endif

#ifdef BUILD_SVG_LOADER
    il2LoadSVG,
#endif

    il2LoadANI,  il2LoadARGB, il2LoadFF, il2LoadICO, il2LoadLBM, il2LoadPNM, il2LoadTGA, il2LoadXBM, 

    // il2LoadXPM,

#ifdef BUILD_BZ2_LOADER
    il2LoadBZ2,
#endif

#ifdef BUILD_ID3_LOADER
    il2LoadID3,
#endif

#ifdef BUILD_J2K_LOADER
    il2LoadJ2K,
#endif

#ifdef BUILD_LZMA_LOADER
    il2LoadLZMA,
#endif

#ifdef BUILD_PS_LOADER
    il2LoadPS,
#endif

#ifdef BUILD_RAW_LOADER
    il2LoadRAW,
#endif

};
#define LOADER_LENGTH sizeof(loaders)/sizeof(loaders[0])



/* from imlib2-1.12.2/src/lib/file.c __imlib_FileOpen */
FILE* __imlib_FileOpen(const char* path, const char* mode, struct stat* st) {

    FILE* fp;

    for (;;) {

        fp = fopen(path, mode);

        if (fp)
            break;

        if (errno != EINTR)
            break;
    }

    /* Only stat if all is good and we want to read */
    if (!fp || !st)
        goto done;

    if (mode[0] == 'w')
        goto done;

    if (fstat(fileno(fp), st) < 0) {
        fclose(fp);
        fp = NULL;
    }

done:
    return fp;
}


/* modified from /imlib2-1.12.2/src/lib/image_tags.c __imlib_GetTag */
ImlibImageTag * __imlib_GetTag(const ImlibImage *im_, const char *key)
{
    struct ImlibImage* im = (struct ImlibImage*)im_;
    ImlibImageTag  *t;

    for (t = im->tags; t; t = t->next)
    {
        if (!strcmp(t->key, key))
            return t;
    }
    return NULL;
}

/* modified from /imlib2-1.12.2/src/lib/image.c __imlib_GetFrame */
ImlibImageFrame * __imlib_GetFrame(ImlibImage *im_)
{
    struct ImlibImage* im = (struct ImlibImage*)im_;

    if (!im->pframe)
        im->pframe = calloc(1, sizeof(ImlibImageFrame));
    return im->pframe;
}

/* from /imlib2-1.12.2/src/lib/image.c __imlib_AllocateData */
uint32_t* __imlib_AllocateData(ImlibImage* im) {

    int w = im->w;
    int h = im->h;

    if (w <= 0 || h <= 0)
        return NULL;

    im->data = malloc(w * h * sizeof(uint32_t));

    return im->data;
}

/* from /imlib2-1.12.2/src/lib/image.c __imlib_FreeData */
void __imlib_FreeData(ImlibImage* im) {

    if (!im->data)
        return;

    free(im->data);

    im->data = NULL;
}

/* from /imlib2-1.12.2/src/lib/image.c __imlib_LoadProgressSetPass */
void __imlib_LoadProgressSetPass(ImlibImage* im, int pass, int n_pass) {

    im->lc->pass   = pass;
    im->lc->n_pass = n_pass;

    im->lc->row = 0;
}

/* from /imlib2-1.12.2/src/lib/image.c __imlib_LoadProgress */
int __imlib_LoadProgress(ImlibImage* im, int x, int y, int w, int h) {

    ImlibLoaderCtx* lc = im->lc;
    int             rc;

    lc->area += w * h;
    lc->pct = (100. * lc->area + .1) / (im->w * im->h);

    rc = !lc->progress(im, lc->pct, x, y, w, h);

    return rc;
}

/* from /imlib2-1.12.2/src/lib/image.c __imlib_LoadProgressRows */
int __imlib_LoadProgressRows(ImlibImage* im, int row, int nrows) {

    ImlibLoaderCtx* lc  = im->lc;
    int             col = 0;
    int             rc  = 0;
    int             pct, nrtot;

    if (nrows > 0) {
        /* Row index counting up */
        nrtot = row + nrows;
        row   = lc->row;
        nrows = nrtot - lc->row;
    } else {
        /* Row index counting down */
        nrtot = im->h - row;
        nrows = nrtot - lc->row;
    }

    pct = (100 * nrtot * (lc->pass + 1)) / (im->h * lc->n_pass);

    if (pct == 100 || pct >= lc->pct + lc->granularity) {
        rc      = !lc->progress(im, pct, col, row, im->w, nrows);
        lc->row = nrtot;
        lc->pct += lc->granularity;
    }

    return rc;
}

bool il2FileContextOpen(ImlibImageFileInfo* fi) {
    return il2FileContextOpenEx(fi, NULL, NULL, 0);
}

/* modified from /imlib2-1.12.2/src/lib/image.c __imlib_FileContextOpen */
bool il2FileContextOpenEx(ImlibImageFileInfo* fi, FILE* fp, const void* fdata, off_t fsize) {

    struct stat st;

    if (fp) {
        fi->fp = fp;
        /* fsize must be given */
    } else if (fdata) {
        /* fsize must be given */
    } else {
        fi->fp = __imlib_FileOpen(fi->name, "rb", &st);
        if (!fi->fp)
            return -1;
        fsize = st.st_size;
    }

    fi->fsize = fsize;

    if (!fdata) {

        fdata = mmap(NULL, fi->fsize, PROT_READ, MAP_SHARED, fileno(fi->fp), 0);

        if (fdata == MAP_FAILED)
            return false;
    }

    fi->fdata = fdata;

    return true;
}

/* modified from /imlib2-1.12.2/src/lib/image.c __imlib_FileContextClose */
void il2FileContextClose(ImlibImageFileInfo* fi) {

    if (fi->fdata) {

        munmap((void*)fi->fdata, fi->fsize);

        fi->fdata = NULL;
    }

    if (fi->fp) {

        fclose(fi->fp);

        fi->fp = NULL;
    }
}


int il2DefaultProgress(ImlibImage* im, char percent, int update_x, int update_y, int update_w, int update_h) {
    return 0;
}

bool il2LoadImageAsRGBA(const char* path, struct ImlibImage* image) {

    bool r = il2LoadImageAsBGRA(path, image);

    if (r) {

        uint32_t* cp = image->data;

        size_t f = image->w * image->h;

        for (size_t i = 0; i < f; i++) {

            // is stored as 0xAABBGGRR
            uint32_t bgra = cp[i];

            cp[i] = ((bgra & 0xff00ff00)) |       // 0xAA__GG__ keep
                    ((bgra & 0x000000ff) << 16) | // 0x______RR << 16
                    ((bgra & 0x00ff0000) >> 16);  // 0x__BB____ >> 16
        }
    }

    return r;
}

bool il2LoadImageAsBGRA(const char* path, struct ImlibImage* image) {

    ImlibLoadArgs      ila = {.pgran = 100, .immed = 1, .nocache = 1};
    ImlibImageFileInfo fi  = {.name = (char*)path};
    struct ImlibImage  im  = {.fi = &fi};

    if(!il2FileContextOpen(im.fi)) {
        printf("Could not open file context\n");
        return false;
    }

    bool rCode = false;

    for(int i = 0; i < LOADER_LENGTH; ++i) {

        ImlibLoadStatus_t ls = loaders[i](&im, ila.immed);

        switch (ls) {

        case IMLIB_STATUS_LOAD_SUCCESS:

            rCode = true;
            i     = LOADER_LENGTH;

            break;

        case IMLIB_STATUS_LOAD_BREAK:

            rCode = false;
            i     = LOADER_LENGTH;
            break;

        case IMLIB_STATUS_LOAD_FAIL:
            break;

        case IMLIB_STATUS_LOAD_OOM:
        case IMLIB_STATUS_LOAD_BADFILE:
        case IMLIB_STATUS_LOAD_BADIMAGE:
        case IMLIB_STATUS_LOAD_BADFRAME:
            printf("Load status was %d\n", ls);
            break;
        }
    }

    il2FileContextClose(im.fi);

    if(rCode) {

        im.fi = NULL;

        *image = im;
    }

    return rCode;
}




