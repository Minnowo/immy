#include "config.h"
#include "Imlib2_Loader.h"

static const char *const _formats[] = { "argb", "arg" };

static struct {
    const unsigned char *data, *dptr;
    unsigned int    size;
} mdata;

static void
mm_init(const void *src, unsigned int size)
{
    mdata.data = mdata.dptr = src;
    mdata.size = size;
}

static void
mm_seek(unsigned int offs)
{
    mdata.dptr = mdata.data + offs;
}

static int
mm_read(void *dst, unsigned int len)
{
    if (mdata.dptr + len > mdata.data + mdata.size)
        return 1;               /* Out of data */

    memcpy(dst, mdata.dptr, len);
    mdata.dptr += len;

    return 0;
}

static int
_load(ImlibImage *im, int load_data)
{
    int             rc;
    int             alpha;
    uint32_t       *imdata;
    int             y;
    const char     *fptr, *row;
    unsigned int    size;

#ifdef WORDS_BIGENDIAN
    int             l;
#endif

    rc = LOAD_FAIL;

    mm_init(im->fi->fdata, im->fi->fsize);

    /* header */

    fptr = im->fi->fdata;
    size = im->fi->fsize > 31 ? 31 : im->fi->fsize;     /* Look for \n in at most 31 byte */
    row = memchr(fptr, '\n', size);
    if (!row)
        goto quit;
    row += 1;                   /* After LF */

    im->w = im->h = alpha = 0;
    sscanf(fptr, "ARGB %i %i %i", &im->w, &im->h, &alpha);

    if (im->w <= 0 || im->h <= 0)
        goto quit;

    rc = LOAD_BADIMAGE;         /* Format accepted */

    if (!IMAGE_DIMENSIONS_OK(im->w, im->h))
        goto quit;

    im->has_alpha = alpha;

    if (!load_data)
        QUIT_WITH_RC(LOAD_SUCCESS);

    /* Load data */

    imdata = __imlib_AllocateData(im);
    if (!imdata)
        QUIT_WITH_RC(LOAD_OOM);

    mm_seek(row - fptr);

    for (y = 0; y < im->h; y++)
    {
        if (mm_read(imdata, 4 * im->w))
            goto quit;

#ifdef WORDS_BIGENDIAN
        for (l = 0; l < im->w; l++)
            SWAP_LE_32_INPLACE(imdata[l]);
#endif
        imdata += im->w;

        if (im->lc && __imlib_LoadProgressRows(im, y, 1))
            QUIT_WITH_RC(LOAD_BREAK);
    }

    rc = LOAD_SUCCESS;

  quit:
    return rc;
}

static int
_save(ImlibImage *im)
{
    int             rc;
    FILE           *f = im->fi->fp;
    const uint32_t *imdata;
    int             y, alpha = 0;
    size_t          nw;

#ifdef WORDS_BIGENDIAN
    uint32_t       *buf = (uint32_t *) malloc(im->w * 4);
    if (!buf)
        QUIT_WITH_RC(LOAD_OOM);
#endif

    rc = LOAD_BADFILE;

    alpha = !!im->has_alpha;

    if (fprintf(f, "ARGB %i %i %i\n", im->w, im->h, alpha) <= 0)
        goto quit;

    imdata = im->data;
    for (y = 0; y < im->h; y++, imdata += im->w)
    {
#ifdef WORDS_BIGENDIAN
        {
            int             x;

            memcpy(buf, imdata, im->w * 4);
            for (x = 0; x < im->w; x++)
                SWAP_LE_32_INPLACE(buf[x]);
            nw = fwrite(buf, 4, im->w, f);
        }
#else
        nw = fwrite(imdata, 4, im->w, f);
#endif
        if (nw != (size_t)im->w)
            goto quit;

        if (im->lc && __imlib_LoadProgressRows(im, y, 1))
            QUIT_WITH_RC(LOAD_BREAK);
    }

    rc = LOAD_SUCCESS;

  quit:
    /* finish off */
#ifdef WORDS_BIGENDIAN
    free(buf);
#endif

    return rc;
}

IMLIB_LOADER(_formats, _load, _save);
