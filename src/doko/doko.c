
// needed for popen
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __unix__
#include <sys/wait.h>
#endif

#include "../config.h"
#include "../darray.h"
#include "doko.h"

int log_level       = LOG_LEVEL;


bool doko_load_with_imlib2(const char* path, Image* im) {

#if !defined(__unix__) || !defined(IMLIB2_ENABLED)

    return 0;

#else

#include <Imlib2.h>

    L_I("About to read image using Imlib2");

    Imlib_Image image;

    image = imlib_load_image_immediately_without_cache(path);

    if (!image) {

        L_E("Error loading with imlib2: %s", imlib_strerror(imlib_get_error()));

        return false;
    }

    imlib_context_set_image(image);

    im->width   = imlib_image_get_width();
    im->height  = imlib_image_get_height();
    im->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    im->mipmaps = 1;

    size_t pixels = im->width * im->height;

    uint32_t* argb = imlib_image_get_data();

    if (!argb) {

        return false;
    }

    // we have to copy the buffer otherwise we can't free from imlib
    im->data = malloc(pixels * sizeof(Color));

    if (!im->data) {

        L_E("Could not make pixel data: %s", strerror(errno));

        return false;
    }

    Color* d = (Color*)im->data;

    for (size_t i = 0; i < pixels; i++) {

        d[i] = (Color){
            .a = (*argb) >> 24,
            .r = (*argb) >> 16,
            .g = (*argb) >> 8,
            .b = (*argb) >> 0,
        };
        argb++;
    }

    imlib_free_image();

    L_I("Loaded with imlib2");

    return true;

#endif
}

int doko_add_image(doko_control_t* ctrl, const char* path_) {

    char* path = doko_strdup(path_);

    if (!path) {

        L_E("Cannot duplicate string '%s'! %s", strerror(errno));

        return -1;
    }

    L_D("Adding file %s", path);

    const char* name = GetFileName(path);

    doko_image_t i = {
        .path        = path,
        .name        = name,
        .rayim       = {0},
        .scale       = 1,
        .rotation    = 0,
        .rebuildBuff = 0,
        .status      = IMAGE_STATUS_NOT_LOADED,
        .srcRect     = {0},
        .dstPos      = {0},
    };

    int newImageIndex = ctrl->image_files.size;

    DARRAY_APPEND(ctrl->image_files, i);

    return newImageIndex;
}


bool doko_loadImage(doko_image_t* image) {

    if (image->status == IMAGE_STATUS_LOADED) {
        return 1;
    }

    if (
#ifdef IMLIB2_ENABLED
        !(doko_load_with_imlib2(image->path, &image->rayim)) &&
#endif

#ifdef DOKO_USE_MAGICK
        !(doko_load_with_magick_stdout(image->path, &image->rayim)) &&
#endif

#ifdef DOKO_USE_FFMPEG
        !(doko_load_with_ffmpeg_stdout(image->path, &image->rayim)) &&
#endif
        true
        ) {

        image->rayim = LoadImage(image->path);
    }

    if (!IsImageReady(image->rayim)) {

        image->status = IMAGE_STATUS_FAILED;

        L_W("Could not load image %s", image->path);

        return 0;
    }

    image->srcRect = (Rectangle){
        0.0,
        0.0,
        image->rayim.width,
        image->rayim.height,
    };
    image->dstPos = (Vector2){0, 0};
    image->status = IMAGE_STATUS_LOADED;

    return 1;
}

void doko_set_image(doko_control_t* ctrl, size_t index) {

    if (index >= ctrl->image_files.size) {

        L_D("Cannot set the image index to %zu", index);

        return;
    }

    ctrl->selected_image = ctrl->image_files.buffer + index;
    ctrl->selected_index = index;
    ctrl->renderFrames   = RENDER_FRAMES;
}

void doko_raylib_log(int msgType, const char *fmt, va_list ap) {

    if (log_level == __LOG_LEVEL_NOTHING)
        return;

    log_level_t level = __LOG_LEVEL_DEBUG;

	switch (msgType)
	{
		case LOG_INFO: level = LOG_LEVEL_INFO; break;
		case LOG_ERROR: level = LOG_LEVEL_ERROR; break;
		case LOG_WARNING: level = LOG_LEVEL_WARN; break;
		case LOG_DEBUG: level = LOG_LEVEL_DEBUG; break;
		default: break;
	}

    FILE* stream = stdout;

    if(level == LOG_LEVEL_ERROR) {
        stream = stderr;
    }

    switch (level) {

    case LOG_LEVEL_NOTHING:
        break;

    case LOG_LEVEL_DEBUG:
        if (log_level <= __LOG_LEVEL_DEBUG) {
            fprintf(stream, "[DEBUG] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_INFO:
        if (log_level <= __LOG_LEVEL_INFO) {
            fprintf(stream, "[INFO] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_WARN:
        if (log_level <= __LOG_LEVEL_WARN) {
            fprintf(stream, "[WARNING] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_ERROR:
        if (log_level <= __LOG_LEVEL_ERROR) {
            fprintf(stream, "[ERROR] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_CRITICAL:

        if (log_level <= __LOG_LEVEL_CRITICAL) {
            fprintf(stream, "[CRITICAL] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;
    }
}

void doko_log(log_level_t level, FILE* stream, const char* fmt, ...) {

    if (log_level == __LOG_LEVEL_NOTHING)
        return;

    va_list ap;

    switch (level) {

    case LOG_LEVEL_NOTHING:
        break;

    case LOG_LEVEL_DEBUG:
        if (log_level <= __LOG_LEVEL_DEBUG) {
            va_start(ap, fmt);
            fprintf(stream, "[DEBUG] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_INFO:
        if (log_level <= __LOG_LEVEL_INFO) {
            va_start(ap, fmt);
            fprintf(stream, "[INFO] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_WARN:
        if (log_level <= __LOG_LEVEL_WARN) {
            va_start(ap, fmt);
            fprintf(stream, "[WARNING] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_ERROR:
        if (log_level <= __LOG_LEVEL_ERROR) {
            va_start(ap, fmt);
            fprintf(stream, "[ERROR] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_CRITICAL:

        if (log_level <= __LOG_LEVEL_CRITICAL) {
            va_start(ap, fmt);
            fprintf(stream, "[CRITICAL] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;
    }
}


// edited from the raylib ImageResizeNN function
bool doko_copy_and_resize_image_nn(
    const Image* image, Image* newimage, int newWidth, int newHeight
) {
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0))
        return false;

    Color* pixels;

    if (image->format != PIXELFORMAT_UNCOMPRESSED_R32G32B32A32) { 
        pixels = LoadImageColors(*image);
    } else { 
        pixels = image->data;
    }

    Color* output = (Color*)RL_MALLOC(newWidth * newHeight * sizeof(Color));

    // EDIT: added +1 to account for an early rounding problem
    int xRatio = (int)((image->width << 16) / newWidth) + 1;
    int yRatio = (int)((image->height << 16) / newHeight) + 1;

    int x2, y2;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            x2 = ((x * xRatio) >> 16);
            y2 = ((y * yRatio) >> 16);

            output[(y * newWidth) + x] = pixels[(y2 * image->width) + x2];
        }
    }

    int format = image->format;

    newimage->data    = output;
    newimage->width   = newWidth;
    newimage->height  = newHeight;
    newimage->mipmaps = 1;
    newimage->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // Reformat 32bit RGBA image to original format
    ImageFormat(
        newimage, format
    );

    if (image->format != PIXELFORMAT_UNCOMPRESSED_R32G32B32A32) {
        UnloadImageColors(pixels);
    }

    return true;
}

bool doko_create_thumbnail( const Image* image, Image* newimage, int newWidth, int newHeight) {

    double ratio;

    if(image->width > image->height) {

        ratio = (double)image->height / image->width;

        return doko_copy_and_resize_image_nn(image, newimage, newWidth, ratio * newHeight);
    }
    else {
        ratio = (double)image->width / image->height;

        return doko_copy_and_resize_image_nn(image, newimage, ratio * newWidth, newHeight);
    }

    return true;
}



static inline unsigned char add_char_clamp(short a, short b) {
    return (a + b) > 255 ? 255 : (a + b);
}
static inline unsigned char sub_char_clamp(short a, short b) {
    return (a - b) < 0 ? 0 : (a - b);
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

    int w = im->rayim.width;
    int h = im->rayim.height;
    size_t pixels_count = w * h;

    im->rayim.data = pixels;
    im->rayim.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    im->rebuildBuff = 1;

    Color oldPixel = WHITE;
    Color newPixel = WHITE;

    for(int i = 0; i < pixels_count; ++i) {

        oldPixel = pixels[i];

        float lum = (oldPixel.r * GRAYSCALE_COEF_R + oldPixel.g * GRAYSCALE_COEF_G + oldPixel.b * GRAYSCALE_COEF_B) / 255.0f;

        if(lum < 0.5)
            newPixel = BLACK;
        else
            newPixel = WHITE;

        Color error = (Color){
            sub_char_clamp(oldPixel.r, newPixel.r), 
            sub_char_clamp(oldPixel.g, newPixel.g), 
            sub_char_clamp(oldPixel.b, newPixel.b), 
            oldPixel.a
        };

        pixels[i] = newPixel;

        // x + 1
        if (i + 1 < pixels_count) {
            int k       = i + 1;
            pixels[k].r = add_char_clamp(pixels[k].r, (error.r * 7) >> 4);
            pixels[k].g = add_char_clamp(pixels[k].g, (error.g * 7) >> 4);
            pixels[k].b = add_char_clamp(pixels[k].b, (error.b * 7) >> 4);
        }

        // x - 1; y + 1
        if(i - 1 + w < pixels_count) {
            int k = i + w - 1;
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




