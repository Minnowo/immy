

#ifndef IMMY_CONFIG_H
#define IMMY_CONFIG_H

#include <raylib.h>

#include "core/core.h"
#include "input.h"

//////////////////////
// Do Not Change
extern InputMapping keybinds[];
extern InputMapping mousebinds[];
extern const size_t KEYBIND_COUNT;
extern const size_t MOUSEBIND_COUNT;

extern const double ZOOM_LEVELS[];
extern const size_t ZOOM_LEVELS_SIZE;
// Do Not Change
//////////////////////


// the default log level
#define LOG_LEVEL __LOG_LEVEL_INFO

// default GLSL version
#ifndef GLSL_VERSION
#define GLSL_VERSION 330
#endif

// if 1, shaders can be used
#define ENABLE_SHADERS 1


// ###########################
// ##### Window Properties ###
// ###########################

// the default window title
// if not changed by cli args
#define WINDOW_TITLE "Immy"

// Default window width
#define START_WIDTH 512

// Default window height 
#define START_HEIGHT 512

// Minimum window width
#define MIN_WINDOW_WIDTH 64

// Minimum window height 
#define MIN_WINDOW_HEIGHT 64

// the target FPS
#define WINDOW_FPS 60

// if set true, the program will close when there is not image
// This only applies at startup
#define DIE_IF_NO_IMAGE 0

// If set to true, render on every frame.
// This ensures max smoothness / no flickering at the cost of more GPU usage.
#define ALWAYS_DO_RENDER true

// When ALWAYS_DO_RENDER is disabled, we might want to redraw every so often.
// Forces a redraw everytime the frame count becomes a multiple of this number.
#define REDRAW_ON_FRAME (WINDOW_FPS * 10)

// When ALWAYS_DO_RENDER is disabled, draw this many frames when needed.
// If the window size changes    -> draw this many frames.
// If the user presses any input -> draw this many frames.
// etc...
// If you see flickering, increase this number.
#define RENDER_FRAMES (3)


// Feature flag for allowing detaching from the terminal.
// If set true, the program can detach from the terminal.
#define ALLOW_DETACH_FROM_TERMINAL true

// Default value for detaching from terminal
#define DETACH_FROM_TERMINAL true


// ###########################
// ##### Image Stuff #########
// ###########################

// Feature flag for loading images in separate threads.
// If set to true, images will be loaded in separate threads.
// Imlib2 will not be used for any loading done in thread.
#define ASYNC_IMAGE_LOADING true

// Number of threads that can be used when making thumbnails.
// This is only used on the thumbnail page.
// This MUST be >= 1.
#define THUMB_ASYNC_LOAD_AMOUNT 1

// When loading thumbnails without async input is blocked.
// If async is disabled, a thumbnail will be loaded 
// every this number of frames. This MUST be > 0.
#define SYNC_IMAGE_LOADING_THUMB_INTERVAL (int)((WINDOW_FPS * 0.5))

// If true thumbnails will be created when the image is loaded.
// Otherwise thumbnails will be created when using thumbnail page.
// (lazy loading)
#define GENERATE_THUMB_WHEN_LOADING_IMAGE true

// If the image is loaded and a thumbnail is not.
// Instead of checking the cache and loading from it,
// generate a thumbnail from the loaded image and update the cache.
#define UPDATE_CACHE_IF_IMAGE_LOADED true

// Cache thumbnails to disk
// They are saved as QOI generally in $HOME/.cache/immy
#define SHOULD_CACHE_THUMBNAILS true

// Use THUMBNAIL_BASE_CACHE_PATH as the thumb cache base directory
#define OVERRIDE_THUMBNAIL_CACHE_PATH false

// If OVERRIDE_THUMBNAIL_CACHE_PATH is true.
// This becomes the thumbnail cache base directory.
#define THUMBNAIL_BASE_CACHE_PATH "/tmp"

// Joins the THUMBNAIL_BASE_CACHE_PATH and the thumb file.
// A cached thumbnail would be in:
//   THUMBNAIL_BASE_CACHE_PATH / THUMBNAIL_CACHE_PATH / SHA256 Of Image Path
#define THUMBNAIL_CACHE_PATH "/.cache/immy/"

// Feature flag for using Imlib2.
// If defined and imlib2 is found at compiletime
// enable using Imlib2 to load images.
// NOTE: Imlib2 can only be used to load images syncronisly, and will not be
//       used for async image loading.
#define USE_IMLIB2


//
// ##### ImageMagick #######
//

// Feature flag for using ImageMagick.
// If defined, ImageMagick will be called to load images.
// Requires image magick's `convert` added to path.
#define IMMY_USE_MAGICK

// Have ImageMagick convert to this format before raylib loads the image.
// Tested options: (in order of my rank of best -> worst)
//   "qoi" -> Very fast, lossless compression, has alpha
//   "bmp" -> Very fast, no compression, has alpha 
//   "png" -> Very slow, lossless compression, has alpha 
//   "jpg" -> Relatively fast, lossy compression, no alpha
#define MAGICK_CONVERT_MIDDLE_FMT "qoi"


//
// ##### FFmpeg #######
//

// Feature flag for using FFmpeg.
// If defined, FFmpeg will be called to load images.
// Requires `ffmpeg` added to path.
#define IMMY_USE_FFMPEG

// Have FFmpeg convert to this format before raylib loads the image.
// Tested options: (in order of my rank of best -> worst)
//   "qoi" -> Very fast, lossless compression, has alpha
//   "bmp" -> Very fast, no compression, has alpha 
//   "png" -> Very slow, lossless compression, has alpha 
#define FFMPEG_CONVERT_MIDDLE_FMT "qoi"

// See https://ffmpeg.org/ffmpeg.html
// This is the value set for the -loglevel and -v flags
#define FFMPEG_VERBOSITY "error"


// ###########################
// ##### Clipboard ###########
// ###########################

// The command which is run to copy images to clipobard for X11.
// Immy will write png bytes to stdin for this command.
// This blocks the GUI thread.
#define X11_COPY_IMAGE_COMMAND "xclip -selection clipboard -target image/png"

// Pasting an image saves to this file before reading the image normally.
#define X11_PASTE_COMMAND_OUTPUT_FILE "/tmp/clipboard.png"

// The command which is run to paste images from the clipboard for X11.
// Must write the image file to X11_PASTE_COMMAND_OUTPUT_FILE.
// This blocks the GUI thread.
#define X11_PASTE_IMAGE_COMMAND "xclip -selection clipboard -target image/png -o > " X11_PASTE_COMMAND_OUTPUT_FILE


// ###########################
// ##### Other ###############
// ###########################

// The order files are sorted when reading a directory.
// Options are:
//  - SORT_ORDER__DEFAULT
//  - SORT_ORDER__NATURAL
#define DEFAULT_SORT_ORDER SORT_ORDER__NATURAL

// When set true, the first image is always centered.
#define CENTER_IMAGE_ON_FIRST_START true

// Size of the background tile pattern.
#define BACKGROUND_TILE_W 128
#define BACKGROUND_TILE_H 128

// Always try and keep this many pixels of the image on screen.
#define IMAGE_INVERSE_MARGIN_X 32
#define IMAGE_INVERSE_MARGIN_Y 32

// Size of thumbnails for the thumbnail page.
#define THUMB_SIZE 256

// The height of the bar 
#define INFO_BAR_HEIGHT 32

// Left margin for bar text
#define INFO_BAR_LEFT_MARGIN 8

// Left margin for the file list
#define FILE_LIST_LEFT_MARGIN INFO_BAR_LEFT_MARGIN

// Show the pixel grid when the scale is bigger than this value
// This value x 100 = % zoom
#define SHOW_PIXEL_GRID_SCALE_THRESHOLD 20 /* 2000% Zoom */

// When true, searches directories recursivly
// When false, does not search directories recursivly
#define SEARCH_DIRS_RECURSIVE true

// The smallest scale value in the ZOOM_LEVELS array
#define SMALLEST_SCALE_VALUE 0.01

// Different scale values which provide a decent default experience.
// This MUST be sorted.
// The first value must be equal to SMALLEST_SCALE_VALUE
#ifdef ZOOM_LEVELS_IMPLEMENTATION

const double ZOOM_LEVELS[] = {
    SMALLEST_SCALE_VALUE,
    0.04, 0.07, 0.10, 0.15,
    0.20, 0.25, 0.30, 0.50,
    0.70, 1.00, 1.50, 2.00,
    3.00, 4.00, 5.00, 6.00,
    7.00, 8.00,

    12.00, 16.00, 20.00, 24.00,
    28.00, 32.00, 36.00, 40.00,
    44.00, 48.00, 52.00, 56.00,
    60.00, 64.00, 68.00, 72.00,
    76.00, 80.00, 84.00, 88.00,
    92.00, 96.00, 100.00,

    104.00, 108.00, 112.00,
    116.00, 120.00, 124.00,
    128.00, 132.00, 136.00,
    140.00, 144.00, 148.00,
    152.00, 156.00, 160.00,
    164.00, 168.00, 172.00,
    176.00, 180.00, 184.00,
    188.00, 192.00, 196.00,
    200.00
};

//////////////////////
// Do Not Change
const size_t ZOOM_LEVELS_SIZE = (sizeof((ZOOM_LEVELS)) / sizeof((ZOOM_LEVELS[0])));
// Do Not Change
//////////////////////

#endif


// ###########################
// ##### Define Colors #######
// ###########################

// The background tile color
#define BACKGROUND_TILE_COLOR_A_RGBA                                           \
    (Color) {                                                                  \
        32, 32, 32, 255                                                        \
    }

// The other backgroudn tile color
#define BACKGROUND_TILE_COLOR_B_RGBA                                           \
    (Color) {                                                                  \
        64, 64, 64, 255                                                        \
    }

// The pixel grid color
#define PIXEL_GRID_COLOR_RGBA                                                  \
    (Color) {                                                                  \
        200, 200, 200, 200                                                     \
    }

// The text color
#define TEXT_COLOR_RGBA                                                        \
    (Color) {                                                                  \
        212, 212, 212, 255                                                     \
    }

// The bar background color
#define BAR_BACKGROUND_COLOR_RGBA                                              \
    (Color) {                                                                  \
        16, 16, 16, 255                                                        \
    }

// The selected color (in the file list)
#define SELECTED_COLOR_RGBA                                                    \
    (Color) {                                                                  \
        96, 64, 64, 255                                                        \
    }

// Magick numbers for calculating grayscale
// Used in the grayscale GPU shader
#define GRAYSCALE_COEF_R 0.299
#define GRAYSCALE_COEF_G 0.587
#define GRAYSCALE_COEF_B 0.114



// ###########################
// ##### Define Input ########
// ###########################

// Feature flag for enabling keybind input.
// If defined, keyboad input works.
#define ENABLE_KEYBOARD_INPUT

// Feature flag for enabling mouse input.
// If defined, mouse input works.
#define ENABLE_MOUSE_INPUT


// Feature flag for enabling file drop.
// If defined, file drop works.
#define ENABLE_FILE_DROP

// Number of keybinds to interpret per frame.
#define KEY_LIMIT 1

// Number of mousebinds to interpret per frame.
#define MOUSE_LIMIT 1

// Time limit for triggering key presses (in seconds)
#define DELAY_MEDIUM (0.17)
#define DELAY_FAST (0.120)
#define DELAY_VFAST (0.08)
#define DELAY_ALMOST_INSTANT (0.03)
#define DELAY_INSTANT (0)

// To find all the keys you can bind see
//  raylib/raylib-5.0/src/raylib.h about line 550 for keys

// Since raylib doesn't have a key for the scroll wheel delta
// Use these to bind a mouse wheel scroll
// MOUSE_WHEEL_FWD = 666
// MOUSE_WHEEL_BWD = 667

// button which quits the program
#define RAYLIB_QUIT_KEY KEY_Q

// Define our keybinds
#ifdef KEYBINDS_IMPLEMENTATION

InputMapping keybinds[] = {

    // KEY,        KEY_CALLBACK,          SCREEN,     LAST_PRESSED,  KEY_TRIGGER_RATE

    // ###########################
    // ##### all pages ###########
    // ###########################
    BIND(KEY_T    , kb_Toggle_Image_Filelist_Screen, SCREEN_ALL, DELAY_MEDIUM ),
    BIND(KEY_ONE  , kb_Goto_Image_Screen           , SCREEN_ALL, DELAY_MEDIUM ),
    BIND(KEY_TWO  , kb_Goto_File_List_Screen       , SCREEN_ALL, DELAY_MEDIUM ),
    BIND(KEY_THREE, kb_Goto_Thumb_Screen           , SCREEN_ALL, DELAY_MEDIUM ),
    BIND(KEY_FOUR , kb_Goto_Keybinds_Screen        , SCREEN_ALL, DELAY_MEDIUM ),

    // ###########################
    // ##### image view page #####
    // ###########################
    //
    BIND(KEY_V | CONTROL_MASK ,kb_Paste_Image_From_Clipboard, SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_NINE             , kb_Dither                   , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_K                , kb_Move_Image_Up            , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_J                , kb_Move_Image_Down          , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H                , kb_Move_Image_Left          , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_L                , kb_Move_Image_Right         , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H | SHIFT_MASK   , kb_Move_Image_Left          , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_L | SHIFT_MASK   , kb_Move_Image_Right         , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_K | SHIFT_MASK   , kb_Zoom_In_Center_Image     , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_J | SHIFT_MASK   , kb_Zoom_Out_Center_Image    , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_S                , kb_Zoom_In_Center_Image     , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_D                , kb_Zoom_Out_Center_Image    , SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H | CONTROL_MASK , kb_Prev_Image               , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_L | CONTROL_MASK , kb_Next_Image               , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_P                , kb_Prev_Image               , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_N                , kb_Next_Image               , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_C | CONTROL_MASK , kb_Copy_Image_To_Clipboard  , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_C                , kb_Fit_Center_Image         , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_C | SHIFT_MASK   , kb_Center_Image             , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_M                , kb_Flip_Horizontal          , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_V                , kb_Flip_Vertical            , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_B                , kb_Toggle_Show_Bar          , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_LEFT_BRACKET     , kb_Cycle_Image_Interpolation, SCREEN_IMAGE, DELAY_MEDIUM), 
    BIND(KEY_O                , kb_Print_Debug_Info         , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_SPACE            , kb_Move_Image_By_Mouse_Delta, SCREEN_IMAGE, DELAY_INSTANT),

#if ENABLE_SHADERS
    BIND(KEY_I, kb_Color_Invert_Shader   , SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_G, kb_Color_Grayscale_Shader, SCREEN_IMAGE, DELAY_MEDIUM),
#else
    BIND(KEY_I, keybind_colorInvert, SCREEN_IMAGE, DELAY_MEDIUM),
#endif


    // ###########################
    // ##### file list page ######
    // ###########################
    BIND(KEY_K                            , kb_Prev_Image      , SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_J                            , kb_Next_Image      , SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_K | SHIFT_MASK               , kb_Prev_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_J | SHIFT_MASK               , kb_Next_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_D | CONTROL_MASK             , kb_Next_Image_By_10, SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_U | CONTROL_MASK             , kb_Prev_Image_By_10, SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_D | CONTROL_MASK | SHIFT_MASK, kb_Next_Image_By_10, SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_U | CONTROL_MASK | SHIFT_MASK, kb_Prev_Image_By_10, SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_P | CONTROL_MASK             , kb_Prev_Image      , SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_N | CONTROL_MASK             , kb_Next_Image      , SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_P | CONTROL_MASK | SHIFT_MASK, kb_Prev_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_N | CONTROL_MASK | SHIFT_MASK, kb_Next_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BINDX(KEY_G | SHIFT_MASK              , kb_Jump_Image_End  , SCREEN_FILE_LIST, KEY_LIMIT, DELAY_MEDIUM),
    BINDX(KEY_G                           , kb_Jump_Image_Start, SCREEN_FILE_LIST, KEY_LIMIT, DELAY_MEDIUM),
    BIND(KEY_ENTER                        ,kb_Goto_Image_Screen, SCREEN_FILE_LIST, DELAY_FAST),


    // ###########################
    // ##### thumbs    page ######
    // ###########################
    BIND(KEY_K,         kb_Thumb_Page_Up       , SCREEN_THUMB_GRID, DELAY_FAST),
    BIND(KEY_J,         kb_Thumb_Page_Down     , SCREEN_THUMB_GRID, DELAY_FAST),
    BIND(KEY_H,         kb_Thumb_Page_Left     , SCREEN_THUMB_GRID, DELAY_FAST),
    BIND(KEY_L,         kb_Thumb_Page_Right    , SCREEN_THUMB_GRID, DELAY_FAST),
    BIND(KEY_ENTER,     kb_Goto_Image_Screen   , SCREEN_THUMB_GRID, DELAY_FAST),

    // ###########################
    // ##### keybind   page ######
    // ###########################
    BIND(KEY_K, kb_Scroll_Keybind_List_Up               , SCREEN_KEYBINDS, DELAY_VFAST),
    BIND(KEY_J, kb_Scroll_Keybind_List_Down             , SCREEN_KEYBINDS, DELAY_VFAST),
    BIND(KEY_K | SHIFT_MASK, kb_Scroll_Keybind_List_Up  , SCREEN_KEYBINDS, DELAY_INSTANT),
    BIND(KEY_J | SHIFT_MASK, kb_Scroll_Keybind_List_Down, SCREEN_KEYBINDS, DELAY_INSTANT),
};

// define mouse input mappings
InputMapping mousebinds[] = {
    BIND(MOUSE_WHEEL_FWD             , kb_Zoom_In_Mouse_Position   , SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD             , kb_Zoom_Out_Mouse_Position  , SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_FWD | SHIFT_MASK, kb_Next_Image               , SCREEN_IMAGE, DELAY_VFAST),
    BIND(MOUSE_WHEEL_BWD | SHIFT_MASK, kb_Prev_Image               , SCREEN_IMAGE, DELAY_VFAST),
    BIND(MOUSE_BUTTON_LEFT           , kb_Move_Image_By_Mouse_Delta, SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_BUTTON_RIGHT          , kb_Move_Image_By_Mouse_Delta, SCREEN_IMAGE, DELAY_INSTANT),

    BIND(MOUSE_WHEEL_FWD             , kb_Prev_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD             , kb_Next_Image      , SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_FWD | SHIFT_MASK, kb_Prev_Image_By_10, SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD | SHIFT_MASK, kb_Next_Image_By_10, SCREEN_FILE_LIST, DELAY_INSTANT),

    BIND(MOUSE_WHEEL_FWD             , kb_Scroll_Keybind_List_Up  , SCREEN_KEYBINDS, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD             , kb_Scroll_Keybind_List_Down, SCREEN_KEYBINDS, DELAY_INSTANT),
};

//////////////////////
// Do Not Change
const size_t KEYBIND_COUNT   = (sizeof(keybinds) / sizeof(keybinds[0]));
const size_t MOUSEBIND_COUNT = (sizeof(mousebinds) / sizeof(mousebinds[0]));
// Do Not Change
//////////////////////

#endif




// ###########################
// ##### Define Resources ####
// ###########################

#ifdef USE_RESOURCE_BUNDLE
    // resource bundle has been created
    // all resources will be compiled into the binary
    // ensure their name is correct
    #ifdef RESOURCE_PATH
        #undef RESOURCE_PATH
    #endif
    #define RESOURCE_PATH "../"
#else
    // the directory which contains the 'resources' folder
    // when doing `make install` this is set to `/opt/immy/`
    // important that this always ends with a / when not empty
    #ifndef RESOURCE_PATH
        #define RESOURCE_PATH ""
    #endif
#endif

// Path to Unifont, the default font
#define UNIFONT_PATH (RESOURCE_PATH "resources/fonts/unifont-15.1.04.otf")

// The spacing for the above font
#define UNIFONT_SPACING 0

// The codepoints to load for the above font at startup
// If the font has utf8 you can put them here
#define CODEPOINT_INITIAL                                                      \
    " !\"#$%&'()*+,-./"                                                        \
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"                       \
    "abcdefghijklmnopqrstuvwxyz{|}"

// ###########################
// ##### Extension Filter ####
// ###########################

// Determine if imlib can be used
#if defined(IMLIB_AVAILABLE) && defined(USE_IMLIB2)
    #define IMLIB2_ENABLED
#endif

#define EXT_WEBP ""
#define EXT_JXL  ""
#define EXT_AVIF ""
#define EXT_TIFF ""
#define EXT_HEIF ""

// Raylib should be able to load these formats
#define RAYLIB_FILE_FORMATS ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm;.psd;.qoi;"

#ifdef IMLIB2_ENABLED
    // with imlib2, we assume it can load these exta formats
    #undef EXT_WEBP 
    #undef EXT_JXL  
    #undef EXT_AVIF 
    #undef EXT_TIFF 
    #undef EXT_HEIF 
    #define EXT_WEBP ".webp;"
    #define EXT_JXL  ".jxl;"
    #define EXT_AVIF ".avif;"
    #define EXT_TIFF ".tiff;"
    #define EXT_HEIF ".heif;"
#endif

#ifdef IMMY_USE_MAGICK
    // with ImageMagick, we assume it can load these exta formats
    #undef EXT_WEBP 
    #undef EXT_JXL  
    #undef EXT_AVIF 
    #undef EXT_TIFF 
    #undef EXT_HEIF 
    #define EXT_WEBP ".webp;"
    #define EXT_JXL  ".jxl;"
    #define EXT_AVIF ".avif;"
    #define EXT_TIFF ".tiff;"
    #define EXT_HEIF ".heif;"
#endif

#ifdef IMMY_USE_FFMPEG
    // with FFmpeg, we assume it can load these exta formats
    #undef EXT_WEBP 
    #undef EXT_JXL  
    #undef EXT_AVIF 
    #undef EXT_TIFF 
    #define EXT_WEBP ".webp;"
    #define EXT_JXL  ".jxl;"
    #define EXT_AVIF ".avif;"
    #define EXT_TIFF ".tiff;"
#endif

// Build the final image search filter
#define IMAGE_FILE_FILTER RAYLIB_FILE_FORMATS EXT_WEBP EXT_JXL EXT_AVIF EXT_TIFF EXT_HEIF



// ###########################
// ##### Commandline Args ####
// ###########################
#define CLI_HELP_f_FLAG "-f       applies borderless window mode"
#define CLI_HELP_B_FLAG "-B       hide the bottom bar"
#define CLI_HELP_b_FLAG "-b       show the bottom bar"
#define CLI_HELP_D_FLAG "-D       keep the terminal"
#define CLI_HELP_d_FLAG "-d       discard the terminal"
#define CLI_HELP_C_FLAG "-C       do not center the first loaded image"
#define CLI_HELP_c_FLAG "-c       center the first loaded image"

#define CLI_HELP_t_FLAG "-t <str> set the window title"
#define CLI_HELP_x_FLAG "-x <int> set the window x pos"
#define CLI_HELP_y_FLAG "-y <int> set the window y pos"
#define CLI_HELP_w_FLAG "-w <int> set the window width"
#define CLI_HELP_h_FLAG "-h <int> set the window height"
#define CLI_HELP_l_FLAG "-l <int 1-6> set the log level"

#define CLI_HELP                                                               \
    "Usage: immy [options] filename/dirname\n"                                 \
    "\n"                                                                       \
    "Basic optionsa:"                                                          \
    "\n  " CLI_HELP_f_FLAG "\n  " CLI_HELP_B_FLAG "\n  " CLI_HELP_b_FLAG       \
    "\n  " CLI_HELP_D_FLAG "\n  " CLI_HELP_d_FLAG "\n  " CLI_HELP_C_FLAG       \
    "\n  " CLI_HELP_c_FLAG "\n  " CLI_HELP_t_FLAG "\n  " CLI_HELP_x_FLAG       \
    "\n  " CLI_HELP_y_FLAG "\n  " CLI_HELP_w_FLAG "\n  " CLI_HELP_h_FLAG       \
    "\n  " CLI_HELP_l_FLAG

#endif

