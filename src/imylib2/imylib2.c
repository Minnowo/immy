
#include "imylib2.h"

#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

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


/* from /imlib2-1.12.2/src/lib/image.c ___imlib_AllocateData */
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

