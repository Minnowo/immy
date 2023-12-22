

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "doko.h"
#include "config.h"
#include "darray.h"

// Different scale values which provide a decent default experience.
// Change as you see fit, just MAKE SURE it is sorted.
const double ZOOM_LEVELS[] = {SMALLEST_SCALE_VALUE, 0.04, 0.07, 0.10, 0.15, 0.20, 0.25, 0.30, 0.50, 0.70, 1.00,
                        1.50, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 12.00, 16.00,
                        20.00, 24.00, 28.00, 32.00, 36.00, 40.00, 44.00, 48.00, 52.00,
                        56.00, 60.00, 64.00, 68.00, 72.00, 76.00, 80.00, 84.00, 88.00,
                        92.00, 96.00, 100.00, 104.00, 108.00, 112.00, 116.00, 120.00,
                        124.00, 128.00, 132.00, 136.00, 140.00, 144.00, 148.00, 152.00,
                        156.00, 160.00, 164.00, 168.00, 172.00, 176.00, 180.00, 184.00,
                        188.00, 192.00, 196.00, 200.00};

#define ZOOM_LEVELS_SIZE (sizeof((ZOOM_LEVELS)) / sizeof((ZOOM_LEVELS[0])))



int doko_loadImage(doko_image_t* image) {

    if(image->status == IMAGE_STATUS_LOADED) {
        return 1;
    }

    image->rayim = LoadImage(image->path);

    if (image->rayim.data == NULL) {
        image->status = IMAGE_STATUS_FAILED;
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
    doko_fitCenterImage(image);

    return 1;
}

void doko_fitCenterImage(doko_image_t* image) {

    int sw = ImageViewWidth;
    int sh = ImageViewHeight;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->scale = fmin((double)sw / iw, (double)sh / ih);

    image->dstPos.x = (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = (sh / 2.0) - (ih * image->scale) / 2.0;
}

void doko_centerImage(doko_image_t* image) {

    int sw = ImageViewWidth;
    int sh = ImageViewHeight;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->dstPos.x = (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = (sh / 2.0) - (ih * image->scale) / 2.0;
}

void doko_ensureImageNotLost(doko_image_t *image) {

    int sw = ImageViewWidth - IMAGE_INVERSE_MARGIN_X;
    int sh = ImageViewHeight - IMAGE_INVERSE_MARGIN_X;
    float iw = (-(image->srcRect.width * image->scale)) + IMAGE_INVERSE_MARGIN_X;
    float ih = (-(image->srcRect.height * image->scale)) + IMAGE_INVERSE_MARGIN_Y;

    if (image->dstPos.x > sw) {
        image->dstPos.x = sw;
    } else if (image->dstPos.x < iw) {
        image->dstPos.x = iw;
    }

    if (image->dstPos.y > sh) {
        image->dstPos.y = sh;
    } else if (image->dstPos.y < ih) {
        image->dstPos.y = ih;
    }
}


void doko_moveScrFracImage(doko_image_t *im, double xFrac, double yFrac) {

    im->dstPos.x += ImageViewWidth * xFrac;
    im->dstPos.y += ImageViewHeight * yFrac;
    doko_ensureImageNotLost(im);
}

void doko_zoomImageCenter(doko_image_t* im, double newScale) {

    double beforeZoom = im->scale;
    double afterZoom = newScale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    #define beforeZoomWidth im->srcRect.width * beforeZoom
    #define beforeZoomHeight im->srcRect.height * beforeZoom

    #define afterZoomWidth im->srcRect.width * afterZoom
    #define afterZoomHeight im->srcRect.height * afterZoom

    im->dstPos.x -= (int)((afterZoomWidth) - (beforeZoomWidth)) >> 1;
    im->dstPos.y -= (int)((afterZoomHeight) - (beforeZoomHeight)) >> 1;
    im->scale = afterZoom;

    doko_ensureImageNotLost(im);
}


void doko_zoomImageOnPoint(doko_image_t* im, double newScale, int x, int y) {

    double beforeZoom = im->scale;
    double afterZoom = newScale;

    if(afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    double scaleRatio =
        (im->srcRect.width * beforeZoom) / (im->srcRect.width * afterZoom);

    double mouseOffsetX = x - im->dstPos.x;
    double mouseOffsetY = y - im->dstPos.y;

    im->dstPos.x -= (int)((mouseOffsetX / scaleRatio) - mouseOffsetX);
    im->dstPos.y -= (int)((mouseOffsetY / scaleRatio) - mouseOffsetY);
    im->scale = afterZoom;

    doko_ensureImageNotLost(im);
}



void doko_zoomImageCenterFromClosest(doko_image_t* im, bool zoomIn) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {
        if (index + 1 < ZOOM_LEVELS_SIZE) {
            index++;
        }
    } else if (index != 0) {
        index--;
    }

    doko_zoomImageCenter(im, ZOOM_LEVELS[index]);
}

void doko_zoomImageOnPointFromClosest(doko_image_t* im, bool zoomIn, int x, int y) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {
        if (index + 1 < ZOOM_LEVELS_SIZE) {
            index++;
        }
    } else if (index != 0) {
        index--;
    }

    doko_zoomImageOnPoint(im, ZOOM_LEVELS[index], x, y);
}

char *doko_strdup(const char *str) {

#ifdef _POSIX_C_SOURCE

    return strdup(str);

#else

    const int len = strlen(str) + 1;

    char *newstr = malloc(len);

    if (!newstr) {
        return NULL;
    }

    memcpy(newstr, str, len);

    return newstr;

#endif
}

void doko_error(int status, int error, const char *fmt, ...) {
    va_list ap;

    fflush(stdout);

    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);

    va_end(ap);

    if (error != 0)
        fprintf(stderr, "%s%s", fmt != NULL ? ": " : "", strerror(error));

    fputc('\n', stderr);

    if (status != 0) exit(status);
}



void set_image(doko_control_t* ctrl, size_t index) {

    if(index >= ctrl->image_files.size) {
        return doko_error(EXIT_SUCCESS, 0,
                          "Cannot set index %zu, which is out of bounds",
                          index);
    }

    ctrl->selected_image = ctrl->image_files.buffer + index;
    ctrl->selected_index = index;
    ctrl->renderFrames = RENDER_FRAMES;
}