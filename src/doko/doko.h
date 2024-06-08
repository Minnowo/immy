

#ifndef DOKO_H
#define DOKO_H

#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "../darray.h"


// MUST be larger than 96
// since we rely on having 96 bytes
// when generating cache path
#if FILENAME_MAX < 256
    #define DOKO_PATH_MAX 256
#else
    #define DOKO_PATH_MAX FILENAME_MAX
#endif

#define FALLTHROUGH /* fall through */

#define PIPE_READ 0
#define PIPE_WRITE 1

#define BYTES_TO_MB(bytes) ((double)(bytes) / (1024 * 1024))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define __LOG_LEVEL_DEBUG 1
#define __LOG_LEVEL_INFO 2
#define __LOG_LEVEL_WARN 3
#define __LOG_LEVEL_ERROR 4
#define __LOG_LEVEL_CRITICAL 5
#define __LOG_LEVEL_NOTHING 6

#define L_D(...) doko_log(__LOG_LEVEL_DEBUG, stdout, __VA_ARGS__)
#define L_I(...) doko_log(__LOG_LEVEL_INFO, stdout, __VA_ARGS__)
#define L_W(...) doko_log(__LOG_LEVEL_WARN, stdout, __VA_ARGS__)
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


#define SHIFT_MASK 0x80000000   // 1000 0000 0000 0000 0000 0000 0000 0000
#define CONTROL_MASK 0x40000000 // 0100 0000 0000 0000 0000 0000 0000 0000
#define KEY_MASK ~(SHIFT_MASK | CONTROL_MASK)

#define HAS_CTRL(k) ((((int)(k)) & CONTROL_MASK) != 0)
#define HAS_SHIFT(k) ((((int)(k)) & SHIFT_MASK) != 0)
#define GET_RAYKEY(k) ((k) & KEY_MASK)

typedef enum { 
    MOUSE_WHEEL_FWD = 666,
    MOUSE_WHEEL_BWD = 667
} MouseWheel;

typedef enum {
    LOG_LEVEL_DEBUG    = __LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO     = __LOG_LEVEL_INFO,
    LOG_LEVEL_WARN     = __LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR    = __LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL = __LOG_LEVEL_CRITICAL,
    LOG_LEVEL_NOTHING  = __LOG_LEVEL_NOTHING
} log_level_t;

typedef enum {
    IMG_PNG,
    IMG_BMP,
    IMG_JPG,
    IMG_QOI,
} ImageFormat_t;

typedef enum {
    SORT_ORDER__DEFAULT, // using strcmp
    SORT_ORDER__NATURAL, // using strnatcmp

} str_compare_t;

typedef enum {
    IMAGE_STATUS_NOT_LOADED,
    IMAGE_STATUS_LOADING,
    IMAGE_STATUS_LOADED,
    IMAGE_STATUS_FAILED
} image_status_t;

typedef enum {
    DOKO_SCREEN_IMAGE = 0,
    DOKO_SCREEN_FILE_LIST,
    DOKO_SCREEN_THUMB_GRID,
    DOKO_SCREEN_KEYBINDS,

    DOKO_SCREEN__START = DOKO_SCREEN_IMAGE,
    DOKO_SCREEN__END   = DOKO_SCREEN_KEYBINDS,
    DOKO_SCREEN__ALL   = -1

} doko_screen_t;

typedef enum {
    DOKO_COLOR_PANEL = 0,

} doko_panel_t;

// max width to render for the screen column on the keybinds page
#define STRLEN_SCREEN_STR 17

// max width to render for the key string column on the keybinds page
#define STRLEN_KEY_STR 20

// a wrapper around the raylib image
// holds lots of extra data and the image itself
typedef struct doko_image {

        // absolute path to the image
        char* path;

        // the filename (this is path + some value) DO NOT FREE
        const char* name;

        image_status_t status;
        image_status_t thumb_status;
        Image          rayim;
        Image          thumb;
        Rectangle      srcRect;
        Vector2        dstPos;

        double scale;
        double rotation;

        TextureFilter interpolation;

        bool rebuildBuff; // updates the Texture2D
        bool applyGrayscaleShader;
        bool applyInvertShader;
        bool thumbIsCached;
        bool panels[1];

} doko_image_t;

// define dynamic array types
DARRAY_DEF(dimage_arr, doko_image_t*);
DARRAY_DEF(dint_arr, int*);
DARRAY_DEF(dbyte_arr, unsigned char*);
DARRAY_DEF(Texture2D_dynamic_arr, Texture2D*);

typedef struct doko_message {
        char*  message;
        bool   free_when_done;
        size_t show_for_frames;
} doko_message_t;

typedef struct doko_config {

        char* cacheDir;
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

        // these are updated on every frame
        size_t  frame;          // keeps track of the current frame
        Vector2 lastMouseClick; // keeps track of the last mouse location

        int renderFrames;   // renders at least this many frames
        int selected_index; // index of selected_image from image_files

        doko_image_t* selected_image; // the selected image;
                                      // points to the image_files;

        doko_message_t message;      // message to show to the user
        dimage_arr_t   image_files;  // images files loaded or not
        doko_config_t  config;       // runtime settings

        // for keeping state on the keybinds page
        size_t keybindPageScroll;
        size_t thumbPageScroll;

        // which screen the user is on
        doko_screen_t screen;

        // function used for filename comparison
        str_compare_t filename_cmp;

} doko_control_t;

extern log_level_t log_level; // global for the log level
extern char*       thumbCachePath;

static inline unsigned char add_char_clamp(short a, short b) {
    return (a + b) > 255 ? 255 : (a + b);
}

static inline unsigned char sub_char_clamp(short a, short b) {
    return (a - b) < 0 ? 0 : (a - b);
}

// 
// string functions
// 
char* dokoStrdup(const char* str);  // just regular strdup
char* dokoEstrdup(const char* str); // strdup but dies instead of returning null

// strdup but adds n extra bytes to the string
char* dokoStrdupn(const char* str, size_t n, size_t* len_);

// join a b with sep
char* dokoStrJoin(const char* a, const char* b, const char* sep);

// joins a b with sep into buf
bool dokoStrJoinInto(
    char* restrict buf, size_t bufSize, const char* a, const char* b,
    const char* sep
);

// strcmp for use with qsort
int dokoQsortStrcmp(const void* a, const void* b);

// natstrcmp for use with qsort
int dokoQsortNatstrcmp(const void* a, const void* b);

const char* dokoGetCacheDirectory();
char* dokoGetThumbPath(const doko_config_t* conf, const char* imagePath);


/**
 * Loads an image and sets some default values.
 * The function returns 0 on success and non-zero on failure.
 */
bool dokoLoadImage(doko_image_t* im);

/**
 * Sets the image to this index if possible
 */
void doko_set_image(doko_control_t* ctrl, size_t index);

/**
 * Append an image to the array 
 * Return the new image index or -1 if there is an error
 */
int doko_add_image(doko_control_t* ctrl, const char* path_);

/**
 * Prints a message to a stream if the log level is above or equal to the set
 * log level
 */
void doko_log(log_level_t level, FILE* stream, const char* fmt, ...);

/**
 * Log function to replace the raylib log function
 */
void doko_raylib_log(int msgType, const char *text, va_list args);

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

/**
 * Gets a pretty name for the TextureFilter
 */
const char* doko_get_interpolation_pretty_text(TextureFilter tf);

/**
 * Copy an image to clipboard
 */
bool doko_copy_image_to_clipboard(doko_image_t* im);

/**
 * Paste an image from the clipboard, creating a new image item in the array
 * Returns the new image index or -1 if there is an error
 */
int doko_paste_image_from_clipboard(doko_control_t* ctrl);

/**
 * Return a resized copy of the image using nearest neighbour algorithm
 */
bool doko_copy_and_resize_image_nn(const  Image* image, Image* newimage, int newWidth, int newHeight);

/**
 * Create a thumbnail image from the given image
 */
bool dokoCreateThumbnail(const Image* image, Image* newimage, int newWidth, int newHeight);

/**
 * Loads an image using image magick through stdout.
 * This method will call 'convert', and convert the image to
 * MAGICK_CONVERT_MIDDLE_FMT before loading it using raylib
 */
bool doko_load_with_magick_stdout(const char* path, Image* im);

/**
 * Loads an image using ffmpeg through stdout.
 * This method will call 'ffmpeg', and convert the image to
 * FFMPEG_CONVERT_MIDDLE_FMT before loading it using raylib
 */
bool doko_load_with_ffmpeg_stdout(const char* path, Image* im);

/**
 * This method will load an image using imlib2.
 * Requires imlib2 to be linked.
 */
bool doko_load_with_imlib2(const char* path, Image* im);


/**
 * Apply a black and white floyd steinburg dither to the image
 */
void doko_dither_image(doko_image_t* im);

bool  dokoGetOrCreateThumb(doko_image_t* im);
char* dokoGetCachedPath(const char* path);
bool  dokoCreateDirectory(const char* path);
bool  dokoSaveThumbnail(const doko_image_t* im);
bool dokoSaveThumbnailAt(const doko_image_t* im, const char* path);
bool  dokoSaveQOI(Image image, const char* path);

/**
 * Update the cacheDir path in the config
 */
void dokoSetCacheDir(doko_config_t* conf, const char* path);

// #####################
// Async Functions Begin
// #####################

/**
 * Begin loading the image without blocking
 */
bool doko_async_load_image(const doko_image_t* im);

/**
 * Checks if the image is in the internal map,
 * Basically returns true if doko_async_load_image was called
 */
bool doko_async_has_image(const doko_image_t* im);

/**
 * Gets the image when it is ready
 * Returns true if the image is done loading
 */
bool doko_async_get_image(doko_image_t* im);

/**
 */
bool doko_async_create_thumbnails(doko_control_t* ctrl);


#endif

