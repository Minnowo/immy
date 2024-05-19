

#ifndef DOKO_H
#define DOKO_H

#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "darray.h"

#define FALLTHROUGH

#define BYTES_TO_MB(bytes) ((double)(bytes) / (1024 * 1024))

#define ImageViewWidth (GetScreenWidth())
#define ImageViewHeight ((GetScreenHeight()) - INFO_BAR_HEIGHT)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define __LOG_LEVEL_DEBUG 1
#define __LOG_LEVEL_INFO 2
#define __LOG_LEVEL_WARN 3
#define __LOG_LEVEL_ERROR 4
#define __LOG_LEVEL_CRITICAL 5
#define __LOG_LEVEL_NOTHING 6

#define L_D(...) doko_log(__LOG_LEVEL_DEBUG, stdout, __VA_ARGS__)
#define L_I(...) doko_log(__LOG_LEVEL_INFO, stdout,  __VA_ARGS__)
#define L_W(...) doko_log(__LOG_LEVEL_WARN, stdout,  __VA_ARGS__)
#define L_E(...) doko_log(__LOG_LEVEL_ERROR, stderr, __VA_ARGS__)
#define L_C(...) doko_log(__LOG_LEVEL_CRITICAL, stderr, __VA_ARGS__)

#define DIE(...)                                                               \
    do {                                                                       \
        L_E(__VA_ARGS__);                                                      \
        fflush(stderr);                                                        \
        exit(1);                                                               \
    } while (0)

#define DIE_IF_NULL(i, ...)                                                    \
    do {                                                                       \
        if ((i) == NULL) {                                                     \
            DIE(__VA_ARGS__);                                                  \
        }                                                                      \
    } while (0)

typedef enum {
    LOG_LEVEL_DEBUG = __LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO = __LOG_LEVEL_INFO,
    LOG_LEVEL_WARN = __LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR = __LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL = __LOG_LEVEL_CRITICAL,
    LOG_LEVEL_NOTHING = __LOG_LEVEL_NOTHING
} log_level_t;

typedef enum {
    SORT_ORDER__DEFAULT, // using strcmp
    SORT_ORDER__NATURAL, // using strnatcmp

} str_compare_t;

typedef enum {
    IMAGE_STATUS_NOT_LOADED,
    IMAGE_STATUS_LOADED,
    IMAGE_STATUS_FAILED
} image_status_t;

typedef enum {
    DOKO_SCREEN_IMAGE = 0,
    DOKO_SCREEN_FILE_LIST,
    DOKO_SCREEN_KEYBINDS,

    DOKO_SCREEN__START = DOKO_SCREEN_IMAGE,
    DOKO_SCREEN__END = DOKO_SCREEN_KEYBINDS,
    DOKO_SCREEN__ALL = -1

} doko_screen_t;

// len("SCREEN_FILE_LIST")
#define STRLEN_SCREEN_STR 16


// a wrapper around the raylib image
// holds lots of extra data and the image itself
typedef struct doko_image {

    // absolute path to the image
    char* path;

    // the filename (this is path + some value) DO NOT FREE
    const char* name;

    image_status_t status;
    Image rayim;
    Rectangle srcRect;
    Vector2 dstPos;

    double scale;
    double rotation;

    bool rebuildBuff; // updates the Texture2D
    bool applyGrayscaleShader;
    bool applyInvertShader;

} doko_image_t;


// define dynamic array types
DARRAY_DEF(dimage_arr, doko_image_t*);
DARRAY_DEF(dint_arr, int*);
DARRAY_DEF(dbyte_arr, unsigned char*);


typedef struct doko_config {

    const char* window_title;

    int window_x; 
    int window_y;
    int window_width; 
    int window_height;
    int window_min_width; 
    int window_min_height;
    int window_flags;

    bool center_image_on_start;
    bool terminal;
    bool set_win_position;
    bool show_bar;

} doko_config_t;


// the main control data
// a singleton is passed to every keypress callback
typedef struct doko_control {

    doko_config_t config;

    // images files loaded or not
    dimage_arr_t image_files;

    // the current image (points to the image_files above)
    doko_image_t *selected_image;

    // index of selected_image from image_files
    int selected_index;

    // keeps track of the last mouse location
    // this is needed for proper dragging of the image with the mouse
    Vector2 lastMouseClick;

    // renders at least this many frames
    // before trying to avoid drawing stuff
    int renderFrames;

    // keeps track of the current frame
    size_t frame;

    // for keeping state on the keybinds page
    size_t keybindPageScroll;

    // which screen the user is on
    doko_screen_t screen;

    str_compare_t filename_cmp; 

} doko_control_t;



/**
 * Loads an image and sets some default values.
 * The function returns 0 on success and non-zero on failure.
 */
int doko_loadImage(doko_image_t* im);

/**
 * Sets the image position and scale so that it centers on the screen.
 */
void doko_fitCenterImage(doko_image_t* im);

/**
 * Sets the image position so that it centers on the screen.
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
 * Moves the image by the given xFrac and yFrac fractions of the screen.
 * xFrac = 1/5 moves the image 1/5th of the horizontal screen.
 */
void doko_moveScrFracImage(doko_image_t *im, double xFrac, double yFrac);

/**
 * Implementation of strdup, because yes
 */
char* doko_strdup(const char* str);

/**
 * Implementation of strdup but adds n extra bytes to the string
 * If len_ is passed returns the length of the new string
 */
char *doko_strdupn(const char *str, size_t n, size_t* len_);

/**
 * Sets the image to this index if possible
*/
void set_image(doko_control_t* ctrl, size_t index);

/**
 * Str compare for use with qsort
*/
int doko_qsort_strcmp(const void *a, const void *b);

/**
 * Str natural compare for use with qsort
*/
int doko_qsort_natstrcmp(const void *a, const void *b);

/**
 * Prints a message to a stream if the log level is above or equal to the set log level
 */
void doko_log(log_level_t level, FILE* stream, const char *fmt, ...);

/**
 * Gets a pretty name for the screen
 */
const char* get_pretty_screen_text(doko_screen_t screen);

/**
 * Gets a pretty name for raylib keys
 */
const char* get_key_to_pretty_text(int key);

/**
 * Gets a pretty name for raylib keys
 */
const char* get_mouse_to_pretty_text(int key);

// the max length of the output of get_key_to_pretty_text and get_mouse_to_pretty_text
// which is len("MOUSE_BUTTON_RIGHT") plus 2 for C and S 
#define STRLEN_KEY_STR 20


inline static bool str_likely_means_true(const char* str) {
    return strncmp(str, "true", 5) == 0 || atoi(str) != 0;
} 

#endif
