

#ifndef DOKO_H
#define DOKO_H

#include <raylib.h>

#include "darray.h"

typedef enum {
    IMAGE_STATUS_NOT_LOADED,
    IMAGE_STATUS_LOADED
} image_status;

typedef struct doko_image {
    char* path;
    size_t nameOffset; // path + this gives name str

    image_status status;
    Image rayim;
    Rectangle srcRect;
    Vector2 dstPos;

    double scale;

} doko_image_t;


DARRAY_DEF(dimage_arr, doko_image_t*);

#define ImageViewWidth GetScreenWidth()
#define ImageViewHeight (GetScreenHeight() - INFO_BAR_HEIGHT)

/**
 * Loads an image and sets some default values.
 * The function returns 0 on success and non-zero on failure.
 */
int doko_loadImage(doko_image_t* im);

/**
 * Sets the image position and scale so that it centers on the screen.
 */
void doko_centerImage(doko_image_t* im);

/**
 * Ensures the image is not lost offscreen by moving it within IMAGE_INVERSE_MARGIN.
 */
void doko_ensureImageNotLost(doko_image_t* im);

/**
 * Scales the image around its center to the new scale.
 */
void doko_zoomImageCenter(doko_image_t* im, double newScale);

/**
 * Scales the image at the given (x, y) point to the new scale.
 */
void doko_zoomImageOnPoint(doko_image_t* im, double newScale, int x, int y);

/**
 * Scales the image around its center to the next closest zoom in ZOOM_LEVELS.
 */
void doko_zoomImageCenterFromClosest(doko_image_t* im, bool zoomIn);

/**
 * Scales the image at the given (x, y) point to the next closest zoom in ZOOM_LEVELS.
 */
void doko_zoomImageOnPointFromClosest(doko_image_t* im, bool zoomIn, int x, int y);

/**
 * Moves the image by the given xFrac and yFrac fractions of the screen.
 * xFrac = 1/5 moves the image 1/5th of the horizontal screen.
 */
void doko_moveScrFracImage(doko_image_t *im, double xFrac, double yFrac);

/**
 * Implementation of strdup, because yes
 */
char* doko_strdup(const char* str);

/**
 * Prints an error
 */
void doko_error(int status, int error, const char *fmt, ...);

#endif