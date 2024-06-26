
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../external/miniz.h"
#include "external/qoi.h" // from raylib
#include "raylib.h"

#include "../config.h"
#include "core.h"

// must come after config.h
#if defined(IMYLIB2_AVAILABLE) && USE_IMYLIB2
#include <imylib2.h>
#endif



bool iLoadImage(ImmyImage_t* im) {

    if (im->status == IMAGE_STATUS_LOADED)
        return true;

#ifdef IMYLIB2_H

    L_D("Using imylib2 to load image.");

    struct ImlibImage il2Image;

    if (il2LoadImageAsRGBA(im->path, &il2Image)) {

        im->rayim.data = il2Image.data;
        im->rayim.width = il2Image.w;
        im->rayim.height = il2Image.h;
        im->rayim.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        im->rayim.mipmaps = 1;
    }
    else

#endif

    if (
#ifdef IMLIB2_ENABLED
            !(iLoadImageWithImlib2(im->path, &im->rayim)) &&
#endif

#ifdef IMMY_USE_MAGICK
        !(iLoadImageWithMagick(im->path, &im->rayim)) &&
#endif

#ifdef IMMY_USE_FFMPEG
        !(iLoadImageWithFFmpeg(im->path, &im->rayim)) &&
#endif
        true) {

        // raylibs load image checks file extension
        // so if it ends with .kra don't bother having raylib load it
        if (!iEndsWith(im->path, ".kra", 1))
            im->rayim = LoadImage(im->path);
    }

    if (!IsImageReady(im->rayim) &&
        // krita is low priority here
        !iLoadKritaImage(im->path, &im->rayim)) {

        im->status = IMAGE_STATUS_FAILED;

        L_W("Could not load image %s", im->path);

        return false;
    }

    im->srcRect = (Rectangle){
        0.0,
        0.0,
        im->rayim.width,
        im->rayim.height,
    };
    im->dstPos = (Vector2){0, 0};
    im->status = IMAGE_STATUS_LOADED;

#if GENERATE_THUMB_WHEN_LOADING_IMAGE
    iGetOrCreateThumb(im);
#endif

    return true;
}

bool iCreateThumbnail(const Image* im, Image* newim, int newW, int newH) {

    double ratio;
    bool   result;

    if (im->width > im->height) {

        ratio = (double)im->height / im->width;

        result = iCopyAndResizeImageNN(im, newim, newW, ratio * newH);

    } else {

        ratio = (double)im->width / im->height;

        result = iCopyAndResizeImageNN(im, newim, ratio * newW, newH);
    }

    if (!result)
        memset(newim, 0, sizeof(*newim));

    return result;
}

// edited from the raylib ImageResizeNN function
bool iCopyAndResizeImageNN(const Image* im, Image* newim, int newW, int newH) {
    // Security check to avoid program crash
    if ((im->data == NULL) || (im->width == 0) || (im->height == 0))
        return false;

    Color* pixels;

    if (im->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        pixels = LoadImageColors(*im);
    } else {
        pixels = im->data;
    }

    Color* output = (Color*)RL_MALLOC(newW * newH * sizeof(Color));

    // EDIT: added +1 to account for an early rounding problem
    int xRatio = (int)((im->width << 16) / newW) + 1;
    int yRatio = (int)((im->height << 16) / newH) + 1;

    int x2, y2;

    for (int y = 0; y < newH; y++) {

        y2 = ((y * yRatio) >> 16);

        for (int x = 0; x < newW; x++) {

            x2 = ((x * xRatio) >> 16);

            output[(y * newW) + x] = pixels[(y2 * im->width) + x2];
        }
    }

    newim->data    = output;
    newim->width   = newW;
    newim->height  = newH;
    newim->mipmaps = 1;
    newim->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    if (im->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        UnloadImageColors(pixels);

    return true;
}

bool iGetOrCreateThumb(ImmyImage_t* im) {

    return iGetOrCreateThumbEx(im, false);
}

bool iGetOrCreateThumbEx(ImmyImage_t* im, bool createOnly) {

    if (im->thumb_status == IMAGE_STATUS_LOADED)
        return true;

#if !SHOULD_CACHE_THUMBNAILS

    if (iCreateThumbnail(&im->rayim, &im->thumb, THUMB_SIZE, THUMB_SIZE)) {

        im->thumb_status = IMAGE_STATUS_LOADED;

        return true;
    }

    return false;
#else

    // if we've already loaded the image,
    // we can just create a thumbnail.
    if (im->status == IMAGE_STATUS_LOADED) {

        if (iCreateThumbnail(&im->rayim, &im->thumb, THUMB_SIZE, THUMB_SIZE)) {

            im->thumb_status = IMAGE_STATUS_LOADED;

#    if UPDATE_CACHE_IF_IMAGE_LOADED
            iSaveThumbnail(im);
#    endif
            return true;
        }
    }

    if (createOnly) {
        return false;
    }

    char* thumbPath = iGetCachedPath(im->path);

    L_D("Trying to read thumb from: %s", thumbPath);

    qoi_desc desc;

    void* rgba_pixels = qoi_read(thumbPath, &desc, 0);

    if (!rgba_pixels) {

        L_D("Could not read thumb from cache");

        bool r = false;

        if (im->status != IMAGE_STATUS_LOADED) {
            FALLTHROUGH;
        } else if (!iCreateThumbnail(&im->rayim, &im->thumb, THUMB_SIZE, THUMB_SIZE)) {
            FALLTHROUGH;
        } else {

            r = true;

            im->thumb_status = IMAGE_STATUS_LOADED;

            iSaveThumbnailAt(im, thumbPath);
        }

        free(thumbPath);

        return r;
    }

    L_D("Cache hit for thumbnail");

    free(thumbPath);

    im->thumb.width   = desc.width;
    im->thumb.height  = desc.height;
    im->thumb.data    = rgba_pixels;
    im->thumb.mipmaps = 1;

    if (desc.channels == 3) {
        im->thumb.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    } else {
        im->thumb.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    }

    im->thumb_status = IMAGE_STATUS_LOADED;

    return true;
#endif
}

bool iSaveQOI(Image image, const char* path) {

    unsigned char* pixels = image.data;

    int channels = 4;

    bool result   = false;
    bool needFree = false;

    if (image.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8) {

        channels = 3;

    } else if (image.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {

        pixels   = (unsigned char*)LoadImageColors(image);
        needFree = true;
    }

    if (pixels) {

        qoi_desc desc   = {0};
        desc.width      = image.width;
        desc.height     = image.height;
        desc.channels   = channels;
        desc.colorspace = QOI_SRGB;

        result = qoi_write(path, pixels, &desc);
    }

    if (needFree)
        UnloadImageColors((Color*)pixels);

    return result;
}

bool iSaveThumbnailAt(const ImmyImage_t* im, const char* path) {

    if (im->thumb_status != IMAGE_STATUS_LOADED)
        return false;

    /*
       struct stat b;

       if(stat(path, &b) == 0) {

       double timeInterval = 60 * 60 * 24;
       double timeDifference = difftime(time(NULL), b.st_mtime);

       if (timeDifference < timeInterval) {

       L_I("Thumb cache was last modified today. Assuming still fine.");

       return true;
       }
       }
       */

    bool r = false;

    if (iCreateDirectory(path)) {

        L_I("%s: saving thumbnail to %s", __func__, path);

        r = iSaveQOI(im->thumb, path);
    }

    return r;
}

bool iSaveThumbnail(const ImmyImage_t* im) {

    if (im->thumb_status != IMAGE_STATUS_LOADED)
        return false;

    char* cachedPath = iGetCachedPath(im->path);

    DIE_IF_NULL(cachedPath, "%s: Cannot get cache path: %s", __func__, strerror(errno));

    bool r = iSaveThumbnailAt(im, cachedPath);

    free(cachedPath);

    return r;
}

void iDitherImage(ImmyImage_t* im) {

    if (im->status != IMAGE_STATUS_LOADED)
        return;

    if (im->rayim.format >= PIXELFORMAT_COMPRESSED_DXT1_RGB)
        return;

    Color* pixels = LoadImageColors(im->rayim);

    if (pixels == NULL)
        return;

    RL_FREE(im->rayim.data);

    int    w            = im->rayim.width;
    int    h            = im->rayim.height;
    size_t pixels_count = w * h;

    im->rayim.data   = pixels;
    im->rayim.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    im->rebuildBuff  = 1;

    Color oldPixel = WHITE;
    Color newPixel = WHITE;

    for (int i = 0; i < pixels_count; ++i) {

        oldPixel = pixels[i];

        float lum =
            (oldPixel.r * GRAYSCALE_COEF_R + oldPixel.g * GRAYSCALE_COEF_G + oldPixel.b * GRAYSCALE_COEF_B) / 255.0f;

        if (lum < 0.5)
            newPixel = BLACK;
        else
          newPixel = WHITE;

        Color error = (Color
        ){sub_char_clamp(oldPixel.r, newPixel.r),
          sub_char_clamp(oldPixel.g, newPixel.g),
          sub_char_clamp(oldPixel.b, newPixel.b),
          oldPixel.a};

        pixels[i] = newPixel;

        // x + 1
        if (i + 1 < pixels_count) {
          int k = i + 1;
          pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 7) >> 4);
          pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 7) >> 4);
          pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 7) >> 4);
        }

        // x - 1; y + 1
        if (i - 1 + w < pixels_count) {
          int k = i + w - 1;
          pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 3) >> 4);
          pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 3) >> 4);
          pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 3) >> 4);
        }

        // y + 1
        if (i + w < pixels_count) {
          int k = i + w;
          pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 5) >> 4);
          pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 5) >> 4);
          pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 5) >> 4);
        }

        // x + 1; y + 1
        if (i + 1 + w < pixels_count) {
          int k = i + w + 1;
          pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 1) >> 4);
          pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 1) >> 4);
          pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 1) >> 4);
        }
    }
}


bool iLoadKritaImage(const char* path, Image* im) {

    mz_zip_archive zip = {0};

    if (!mz_zip_reader_init_file(&zip, path, 0)) {

        L_E("Could not read Krita file: %s", mz_zip_get_error_string( zip.m_last_error));

        return false;
    }

    const char* KRITA_IM_PATH[2] = {
        "mergedimage.png", // the full res image
        "preview.png"      // a thumbnail image
    };

    for (int i = 0; i < 2; i++) {

        const char* PATH = KRITA_IM_PATH[i];

        size_t uncomp_size;

        void* p = mz_zip_reader_extract_file_to_heap(&zip, PATH, &uncomp_size, 0);

        if (!p)
            continue;

        L_D("Successfully extracted file \"%s\", size %zu\n", PATH, uncomp_size);


        *im = LoadImageFromMemory(".png", p, uncomp_size);

        mz_free(p);

        if(IsImageReady(*im))
            break;
    }

    mz_zip_reader_end(&zip);

    return IsImageReady(*im);
}
