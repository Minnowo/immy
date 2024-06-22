

#ifndef IMMY_H
#define IMMY_H

#include "external/glfw/include/GLFW/glfw3.h"
#include "raylib.h"

#include <stdio.h>
#include <string.h>

#include "../darray.h"
#include "../external/hashmap.h"

///
/// Macro Definitions
///

// MUST be larger than 96.
// This is because we rely on having 96 bytes when generating cache path.
#if FILENAME_MAX < 256
#    define IMMY_PATH_MAX 256
#else
#    define IMMY_PATH_MAX FILENAME_MAX
#endif

#if IMMY_PATH_MAX < 3 * SHA256_BLOCK_SIZE
#    error "IMMY_PATH_MAX Cannot be less than 3 x SHA256_BLOCK_SIZE"
#endif

// should be defined by CMake
#ifndef SOURCE_PATH_SIZE
#    define SOURCE_PATH_SIZE 0
#endif

// To get the filename for debugging.
#define __FILENAME__ (((char*)__FILE__) + SOURCE_PATH_SIZE)

// Used to indicate fallthrough.
#define FALLTHROUGH /* fall through */

// For reading / writing pipes.
#define PIPE_READ 0
#define PIPE_WRITE 1

#define BYTES_TO_MB(bytes) ((double)(bytes) / (1024 * 1024))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Used for compile time log checks.
#define __LOG_LEVEL_DEBUG 1
#define __LOG_LEVEL_INFO 2
#define __LOG_LEVEL_WARN 3
#define __LOG_LEVEL_ERROR 4
#define __LOG_LEVEL_CRITICAL 5
#define __LOG_LEVEL_NOTHING 6

// Logging helpers.
#define L_D(f, ...) iLog(__LOG_LEVEL_DEBUG, stdout, "%s:%s:%d: " f, __FILENAME__, __func__, __LINE__, ##__VA_ARGS__)
#define L_I(f, ...) iLog(__LOG_LEVEL_INFO, stdout, (f), ##__VA_ARGS__)
#define L_W(f, ...) iLog(__LOG_LEVEL_WARN, stdout, (f), ##__VA_ARGS__)
#define L_E(f, ...) iLog(__LOG_LEVEL_ERROR, stderr, (f), ##__VA_ARGS__)
#define L_C(f, ...) iLog(__LOG_LEVEL_CRITICAL, stderr, (f), ##__VA_ARGS__)

#define DIE(fmt, ...)                                                                                                  \
    do {                                                                                                               \
        L_E((fmt), ##__VA_ARGS__);                                                                                     \
        fflush(stderr);                                                                                                \
        exit(1);                                                                                                       \
    } while (0)

#define DIE_IF_NULL(i, fmt, ...)                                                                                       \
    do {                                                                                                               \
        if ((i) == NULL) {                                                                                             \
            DIE((fmt), ##__VA_ARGS__);                                                                                 \
        }                                                                                                              \
    } while (0)

// For key detection / keybind definitions.
#define SHIFT_MASK 0x80000000   /* 1000 0000 0000 0000 0000 0000 0000 0000 */
#define CONTROL_MASK 0x40000000 /* 0100 0000 0000 0000 0000 0000 0000 0000 */

#define KEY_MASK ~(SHIFT_MASK | CONTROL_MASK)

#define HAS_CTRL(k) ((((int)(k)) & CONTROL_MASK) != 0)
#define HAS_SHIFT(k) ((((int)(k)) & SHIFT_MASK) != 0)
#define GET_RAYKEY(k) ((k) & KEY_MASK)

// Max padding / width on the keybinds page for screen column.
#define STRLEN_SCREEN_STR 17

// Max padding / width on the keybinds page for key column.
#define STRLEN_KEY_STR 20

///
/// Enum Definitions
///

// Since Raylib does not have a mouse forward and backward.
typedef enum { MOUSE_WHEEL_FWD = 666, MOUSE_WHEEL_BWD = 667 } MouseWheel_t;

typedef enum {
    LOG_LEVEL_DEBUG    = __LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO     = __LOG_LEVEL_INFO,
    LOG_LEVEL_WARN     = __LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR    = __LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL = __LOG_LEVEL_CRITICAL,
    LOG_LEVEL_NOTHING  = __LOG_LEVEL_NOTHING
} LogLevel_t;

typedef enum {
    IMG_PNG,
    IMG_BMP,
    IMG_JPG,
    IMG_QOI,
} ImageFormat_t;

typedef enum {
    SORT_ORDER__DEFAULT, // using strcmp
    SORT_ORDER__NATURAL, // using strnatcmp

} StrCompare_t;

typedef enum {
    IMAGE_STATUS_NOT_LOADED,
    IMAGE_STATUS_LOADING,
    IMAGE_STATUS_LOADED,
    IMAGE_STATUS_FAILED,
} ImageLoadStatus_t;

typedef enum {
    SCREEN_IMAGE = 0,
    SCREEN_FILE_LIST,
    SCREEN_THUMB_GRID,
    SCREEN_KEYBINDS,

    // so we can cycle with integer addition
    SCREEN__START = SCREEN_IMAGE,
    SCREEN__END   = SCREEN_KEYBINDS,
    SCREEN_ALL    = -1

} UIScreen_t;

// WIP, not used.
typedef enum {
    COLOR_PANEL = 0,

} UIPanel_t;

// Holds an image and everything about it.
typedef struct ImmyImage {

        // absolute path to the image
        char* path;

        // the filename (this is path + some value) DO NOT FREE
        const char* name;

        ImageLoadStatus_t status;
        ImageLoadStatus_t thumb_status;
        Image             rayim;
        Image             thumb;
        Rectangle         srcRect;
        Vector2           dstPos;

        double scale;
        double rotation;

        TextureFilter interpolation;

        bool rebuildBuff; // updates the Texture2D
        bool updateShaders;
        bool applyGrayscaleShader;
        bool applyInvertShader;
        bool isLoadingForThumbOnly;
        bool panels[1];

} ImmyImage_t;

// Define dynamic array types.
DARRAY_DEF(dImmyImageArr, ImmyImage_t);
DARRAY_DEF(dIntArr, int);
DARRAY_DEF(dByteArr, unsigned char);
DARRAY_DEF(dTexture2DArr, Texture2D);

// Used for showing a message on the status bar.
typedef struct ImmyMessage {
        char*  message;
        bool   free_when_done;
        size_t show_for_frames;
} ImmyMessage_t;

typedef struct ImmyConfig {

        char*       cacheDir;
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

} ImmyConfig_t;

// The main control data.
// A singleton is passed to every keypress callback.
typedef struct ImmyControl {

        // these are updated on every frame
        size_t  frame;          // keeps track of the current frame
        Vector2 lastMouseClick; // keeps track of the last mouse location

        int renderFrames;   // renders at least this many frames
        int selected_index; // index of selected_image from image_files

        ImmyImage_t* selected_image; // the selected image;
                                     // points to the image_files;

        ImmyMessage_t   message;     // message to show to the user
        dImmyImageArr_t image_files; // images files loaded or not
        ImmyConfig_t    config;      // runtime settings

        // for keeping state on the keybinds page
        size_t keybindPageScroll;
        size_t thumbPageScroll;

        // which screen the user is on
        UIScreen_t screen;

        // function used for filename comparison
        StrCompare_t filename_cmp;

} ImmyControl_t;

///
/// Global Variables
///

// The log level used by iLog and iLogRaylib.
extern LogLevel_t log_level;

// Used by image_async.c to map an image to a thread.
extern struct hashmap* asyncMap;

///
/// String functions
///

// Just regular strdup
char* iStrDup(const char* str);

// Strdup but dies instead of returning null
char* ieStrDup(const char* str);

// Strdup but adds n extra bytes to the string
char* inStrDup(const char* str, size_t n, size_t* len_);

// Join a b with sep
char* iStrJoin(const char* a, const char* b, const char* sep);

// Joins a b with sep into buf
bool iStrJoinInto(char* restrict buf, size_t bufSize, const char* a, const char* b, const char* sep);

// strcmp for use with qsort
int iqStrCmp(const void* a, const void* b);

// natstrcmp for use with qsort
int iqNatStrCmp(const void* a, const void* b);

// Gets the cache directory
const char* iGetCacheDirectory();

// Gets the cache location for the given file path.
char* iGetCachedPath(const char* path);

///
/// Image Functions
///

// Loads an image and sets some default values.
// The function returns 0 on success and non-zero on failure.
bool iLoadImage(ImmyImage_t* im);

// Loads an image using ImageMagick.
// This method will call 'convert', and convert the image to
// MAGICK_CONVERT_MIDDLE_FMT before loading it using Raylib.
bool iLoadImageWithMagick(const char* path, Image* im);

// Loads an image using FFmpeg.
// This method will call 'ffmpeg',
// and convert the image to FFMPEG_CONVERT_MIDDLE_FMT before loading it using Raylib.
bool iLoadImageWithFFmpeg(const char* path, Image* im);

// This method will load an image using Imlib2.
// Requires imlib2 to be linked.
bool iLoadImageWithImlib2(const char* path, Image* im);

// Load a thumbnail from the cache or create on from the image.
bool iGetOrCreateThumb(ImmyImage_t* im);
bool iGetOrCreateThumbEx(ImmyImage_t* im, bool createOnly);

// Save a thumbnail in the cache.
bool iSaveThumbnail(const ImmyImage_t* im);

// Save a thumbnail at the given path.
bool iSaveThumbnailAt(const ImmyImage_t* im, const char* path);

// Saves a Raylib image in the QOI format.
bool iSaveQOI(Image image, const char* path);

// Sets the image to this index if possible
void iSetImage(ImmyControl_t* ctrl, size_t index);

// Append an image to the array.
// Return the new image index or -1 if there is an error.
int iAddImage(ImmyControl_t* ctrl, const char* path_);

// Create a thumbnail image from the given image into the other given image.
bool iCreateThumbnail(const Image* image, Image* newimage, int newWidth, int newHeight);

// Apply a black and white floyd steinburg dither to the image.
void iDitherImage(ImmyImage_t* im);

///
/// IO Functions
///

bool iCreateDirectory(const char* path);
void iDetachFromTerminal();

///
/// Logging Functions
///

// Prints a message to a stream if the log level is above or equal to the global loglevel.
void iLog(LogLevel_t level, FILE* stream, const char* fmt, ...);

// Log function to replace the Raylib log function.
void iLogRaylib(int msgType, const char* text, va_list args);

///
/// To String Functions
///

// Gets a pretty name for the screen.
const char* iScreenToStr(UIScreen_t screen);

// Gets a pretty name for Raylib keys.
const char* iKeyToStr(int key);

// Gets a pretty name for Raylib keys.
const char* iMouseToStr(int key);

// Gets a pretty name for the TextureFilter.
const char* iInterpolationToStr(TextureFilter tf);

///
/// Clipboard Functions
///

// Copy an image to clipboard.
bool iCopyImageToClipboard(ImmyImage_t* im);

// Paste an image from the clipboard, creating a new image item in the array.
// Returns the new image index or -1 if there is an error.
int iPasteImageFromClipboard(ImmyControl_t* ctrl);

// Return a resized copy of the image using nearest neighbour algorithm.
bool iCopyAndResizeImageNN(const Image* image, Image* newimage, int newWidth, int newHeight);

///
/// Async Functions
///

// Begin loading the image without blocking.
bool iLoadImageAsync(const ImmyImage_t* im);

// Checks if the image is in the internal map,
// Basically returns true if immy_async_load_image was called.
bool iAsyncHasImage(const ImmyImage_t* im);

// Gets the image when it is ready.
// Returns true if the image is done loading.
bool iGetImageAsync(ImmyImage_t* im);

///
/// Platform Specific Stuff
///

#ifdef __unix__
bool uDetachFromTerminal();
#endif

#ifdef _WIN32
bool wDetachFromTerminal();
#endif

#ifdef X11_AVAILABLE

// Gets the X11 window handle from the output of Raylib's GetWindowHandle().
int xGetWindowHandle(GLFWwindow* handle);

// Sets the Override Redirect X11 window attribute.
void xSetOverrideRedirect(GLFWwindow* handle);

// Grabs the keyboard.
int xUngrabKeyboard(GLFWwindow* handle);

// Ungrabs the keyboard.
int xGrabKeyboard(GLFWwindow* handle);

#endif

///
/// Inline Methods
///

static inline unsigned char add_char_clamp(short a, short b) {
    return (a + b) > 255 ? 255 : (a + b);
}

static inline unsigned char sub_char_clamp(short a, short b) {
    return (a - b) < 0 ? 0 : (a - b);
}

#endif
