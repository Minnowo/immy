
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "raylib.h"
#include "external/qoi.h" // from raylib

#include "../config.h"
#include "doko.h"

bool dokoLoadImage(doko_image_t* im) {

    if (im->status == IMAGE_STATUS_LOADED)
        return true;

    if (
#ifdef IMLIB2_ENABLED
        !(doko_load_with_imlib2(im->path, &im->rayim)) &&
#endif

#ifdef DOKO_USE_MAGICK
        !(doko_load_with_magick_stdout(im->path, &im->rayim)) &&
#endif

#ifdef DOKO_USE_FFMPEG
        !(doko_load_with_ffmpeg_stdout(im->path, &im->rayim)) &&
#endif
        true
        ) {

        im->rayim = LoadImage(im->path);
    }

    if (!IsImageReady(im->rayim)) {

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

#if GENERATE_THUMB_WHEN_LOADING_IMAGE == true
    dokoGetOrCreateThumb(im);
#endif

    return true;
}

bool dokoCreateThumbnail(const Image* im, Image* newim, int newW, int newH) {

    double ratio;
    bool   result;

    if (im->width > im->height) {

        ratio = (double)im->height / im->width;

        result = doko_copy_and_resize_image_nn(im, newim, newW, ratio * newH);

    } else {

        ratio = (double)im->width / im->height;

        result = doko_copy_and_resize_image_nn(im, newim, ratio * newW, newH);
    }

    if (!result)
        memset(newim, 0, sizeof(*newim));

    return result;
}

// edited from the raylib ImageResizeNN function
bool doko_copy_and_resize_image_nn(
    const Image* im, Image* newim, int newW, int newH
) {
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

    for (int y = 0; y < newH; y++)

        for (int x = 0; x < newW; x++) {
            x2 = ((x * xRatio) >> 16);
            y2 = ((y * yRatio) >> 16);

            output[(y * newW) + x] = pixels[(y2 * im->width) + x2];
        }

    int format = im->format;

    newim->data    = output;
    newim->width   = newW;
    newim->height  = newH;
    newim->mipmaps = 1;
    newim->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // Reformat 32bit RGBA image to original format
    ImageFormat(newim, format);

    if (im->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        UnloadImageColors(pixels);

    return true;
}

bool dokoGetOrCreateThumb(doko_image_t* im) {

    if (im->thumb_status == IMAGE_STATUS_LOADED)
        return true;

    char* thumbPath = dokoGetCachedPath(im->path);

    qoi_desc desc;

    void* rgba_pixels = qoi_read(thumbPath, &desc, 0);

    if(!rgba_pixels) {

        if(im->status != IMAGE_STATUS_LOADED)
            return false;

        if (!dokoCreateThumbnail(&im->rayim, &im->thumb, THUMBNAIL_SIZE, THUMBNAIL_SIZE))
            return false;

        im->thumb_status = IMAGE_STATUS_LOADED;

        dokoSaveThumbnail(im);

        return true;
    }

    im->thumb.width = desc.width;
    im->thumb.height = desc.height;
    im->thumb.data = rgba_pixels;
    im->thumb.mipmaps = 1;

    if(desc.channels == 3) {
        im->thumb.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    }
    else {
        im->thumb.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    }

    im->thumb_status = IMAGE_STATUS_LOADED;

    return true;
}


bool dokoSaveQOI(Image image, const char* path) {

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

    if(needFree) 
        UnloadImageColors((Color*)pixels);

    return result;
}

bool dokoSaveThumbnail(const doko_image_t* im) {

    if(im->thumb_status != IMAGE_STATUS_LOADED)
        return false;

    char* cachedPath = dokoGetCachedPath(im->path);

    DIE_IF_NULL(cachedPath, "%s: Cannot get cache path: %s", __func__, strerror(errno));

    /*
    struct stat b;

    if(stat(cachedPath, &b) == 0) {

        time_t currentTime = time(NULL);
        double timeInterval = 60 * 60 * 24;
        double timeDifference = difftime(currentTime, b.st_mtime);

        if (timeDifference < timeInterval) {

            L_I("Thumb cache was last modified today. Assuming still fine.");

            return true;
        } 
    }
    */

    bool r = false;

    if (dokoCreateDirectory(cachedPath)) {

        L_E("%s: Could not create cache directory: %s", __func__,
            strerror(errno));

        L_I("%s: saving thumbnail to %s", __func__, cachedPath);

        r = dokoSaveQOI(im->thumb, cachedPath);
    }

    free(cachedPath);

    return r;
}


void doko_dither_image(doko_image_t* im) {

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
            (oldPixel.r * GRAYSCALE_COEF_R + oldPixel.g * GRAYSCALE_COEF_G +
             oldPixel.b * GRAYSCALE_COEF_B) /
            255.0f;

        if (lum < 0.5)
            newPixel = BLACK;
        else
            newPixel = WHITE;

        Color error = (Color
        ){sub_char_clamp(oldPixel.r, newPixel.r),
          sub_char_clamp(oldPixel.g, newPixel.g),
          sub_char_clamp(oldPixel.b, newPixel.b), oldPixel.a};

        pixels[i] = newPixel;

        // x + 1
        if (i + 1 < pixels_count) {
            int k       = i + 1;
            pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 7) >> 4);
            pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 7) >> 4);
            pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 7) >> 4);
        }

        // x - 1; y + 1
        if (i - 1 + w < pixels_count) {
            int k       = i + w - 1;
            pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 3) >> 4);
            pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 3) >> 4);
            pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 3) >> 4);
        }

        // y + 1
        if (i + w < pixels_count) {
            int k       = i + w;
            pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 5) >> 4);
            pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 5) >> 4);
            pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 5) >> 4);
        }

        // x + 1; y + 1
        if (i + 1 + w < pixels_count) {
            int k       = i + w + 1;
            pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 1) >> 4);
            pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 1) >> 4);
            pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 1) >> 4);
        }
    }
}
