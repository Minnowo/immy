

#ifndef DOKO_CONFIG_H
#define DOKO_CONFIG_H

#include <raylib.h>

#include "doko.h"
#include "input.h"


#define LOG_LEVEL __LOG_LEVEL_INFO

#ifndef GLSL_VERSION
    #define GLSL_VERSION 330
#endif

// if 1, try to load shaders for grayscale and inverting color
#define ENABLE_SHADERS 1



//
// Define window properties
//

#define WINDOW_TITLE "Doko?"
#define WINDOW_FPS 60

// forces a redraw everytime the frame count
// becomes a multiple of this number
#define REDRAW_ON_FRAME (WINDOW_FPS*3)

// when defined always do render
// this will prevent flickering on some systems
#define ALWAYS_DO_RENDER

// how many frames to render when being told to render
// this only applies if ALWAYS_DO_RENDER is not defined
// if you get flickering, increase this number
#define RENDER_FRAMES (1+(int)(WINDOW_FPS/4))

// if 1, try and detach from the terminal
#define DETACH_FROM_TERMINAL 1

// use imlib2
// you will also need to set -lImlib2 in the LDFLAGS of the makefile
#define USE_IMLIB2 0

// if 1 load images using image magick convert before raylib
// requires image magick's `convert` added to path
#define USE_MAGICK_CONVERT 0
// have magick convert to this format before raylib loads the image
#define MAGICK_CONVERT_MIDDLE_FMT "qoi"

// if 1 try and load images using ffmpeg before raylib
// requires `ffmpeg` added to path
#define USE_FFMPEG_CONVERT 0
// have ffmpeg convert to this format before raylib loads the image
#define FFMPEG_CONVERT_MIDDLE_FMT "qoi"
// see https://ffmpeg.org/ffmpeg.html
// this is the value set for the -loglevel and -v flags
#define FFMPEG_VERBOSITY "error"

// window sizes
#define START_WIDTH 512
#define START_HEIGHT 512
#define MIN_WINDOW_WIDTH 64
#define MIN_WINDOW_HEIGHT 64

#define DEFAULT_SORT_ORDER SORT_ORDER__NATURAL

// Size of the background tile pattern
#define BACKGROUND_TILE_W 128
#define BACKGROUND_TILE_H 128

// always try and keep this many pixels of the image on screen
#define IMAGE_INVERSE_MARGIN_X 32
#define IMAGE_INVERSE_MARGIN_Y 32

// define the height of the bar 
#define INFO_BAR_HEIGHT 32
#define INFO_BAR_LEFT_MARGIN 8

#define FILE_LIST_LEFT_MARGIN INFO_BAR_LEFT_MARGIN

// show the pixel grid when the scale is bigger than this value
#define SHOW_PIXEL_GRID_SCALE_THRESHOLD 20

// draw the fps
#define DRAW_FPS 0

// 1 searches directories recursivly
// 0 does not search directories recursivly
#define SEARCH_DIRS_RECURSIVE 1

// the smallest scale value in the ZOOM_LEVELS array
#define SMALLEST_SCALE_VALUE 0.01

// Different scale values which provide a decent default experience.
// Change as you see fit, just MAKE SURE it is sorted.
static const double ZOOM_LEVELS[] = {SMALLEST_SCALE_VALUE,
                                     0.04,
                                     0.07,
                                     0.10,
                                     0.15,
                                     0.20,
                                     0.25,
                                     0.30,
                                     0.50,
                                     0.70,
                                     1.00,
                                     1.50,
                                     2.00,
                                     3.00,
                                     4.00,
                                     5.00,
                                     6.00,
                                     7.00,
                                     8.00,
                                     12.00,
                                     16.00,
                                     20.00,
                                     24.00,
                                     28.00,
                                     32.00,
                                     36.00,
                                     40.00,
                                     44.00,
                                     48.00,
                                     52.00,
                                     56.00,
                                     60.00,
                                     64.00,
                                     68.00,
                                     72.00,
                                     76.00,
                                     80.00,
                                     84.00,
                                     88.00,
                                     92.00,
                                     96.00,
                                     100.00,
                                     104.00,
                                     108.00,
                                     112.00,
                                     116.00,
                                     120.00,
                                     124.00,
                                     128.00,
                                     132.00,
                                     136.00,
                                     140.00,
                                     144.00,
                                     148.00,
                                     152.00,
                                     156.00,
                                     160.00,
                                     164.00,
                                     168.00,
                                     172.00,
                                     176.00,
                                     180.00,
                                     184.00,
                                     188.00,
                                     192.00,
                                     196.00,
                                     200.00};





//
// Define Colors, (r, g, b, a)
//

#define BACKGROUND_TILE_COLOR_A_RGBA                                           \
    (Color){ 32, 32, 32, 255 }

#define BACKGROUND_TILE_COLOR_B_RGBA                                           \
    (Color){ 64, 64, 64, 255 }

#define PIXEL_GRID_COLOR_RGBA                                                  \
    (Color){ 200, 200, 200, 200 }

// used in the grayscale gpu shader
#define GRAYSCALE_COEF_R 0.299
#define GRAYSCALE_COEF_G 0.587
#define GRAYSCALE_COEF_B 0.114



// 
// Define Input
//

#define ENABLE_KEYBOARD_INPUT          // all keyboard input
#define ENABLE_MOUSE_INPUT             // all mouse input
#define ENABLE_FILE_DROP               // file drop into the app

// number of keybinds to interpret per render
#define KEY_LIMIT 3

// number of mouses inputs to interpret per render
#define MOUSE_LIMIT 1

// button which quits the program
#define RAYLIB_QUIT_KEY KEY_Q

// Time limit for triggering key presses (in seconds)
#define DELAY_MEDIUM  (0.2)
#define DELAY_FAST    (0.120)
#define DELAY_VFAST   (0.08)
#define DELAY_INSTANT (0)


// To find all the keys you can bind see
//  raylib/raylib-5.0/src/raylib.h about line 550 for keys

// Since raylib doesn't have a key for the scroll wheel delta
// Use these to bind a mouse wheel scroll
#define MOUSE_WHEEL_FORWARD_BUTTON 666
#define MOUSE_WHEEL_BACKWARD_BUTTON 667

// define keybinds, you can find more methods in the keybind.h file
// define any new ones as you see fit
static InputMapping keybinds[] = {

    // KEY,        KEY_CALLBACK,          SCREEN,     LAST_PRESSED,  KEY_TRIGGER_RATE

    // ###########################
    // ##### all pages ###########
    // ###########################
    {KEY_T, keybind_cycleScreen   , DOKO_SCREEN__ALL, 0, DELAY_MEDIUM },

    // ###########################
    // ##### image view page #####
    // ###########################
    {KEY_K                , keybind_moveImageUp          , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_J                , keybind_moveImageDown        , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_H                , keybind_moveImageLeft        , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_L                , keybind_moveImageRight       , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_H | SHIFT_MASK   , keybind_moveImageLeft        , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_L | SHIFT_MASK   , keybind_moveImageRight       , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_K | SHIFT_MASK   , keybind_zoomInCenterImage    , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_J | SHIFT_MASK   , keybind_zoomOutCenterImage   , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_H | CONTROL_MASK , keybind_prevImage            , DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
    {KEY_L | CONTROL_MASK , keybind_nextImage            , DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
    {KEY_P                , keybind_prevImage            , DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
    {KEY_N                , keybind_nextImage            , DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
    {KEY_C                , keybind_fitCenterImage       , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_M                , keybind_flipHorizontal       , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_V                , keybind_flipVertical         , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_O                , keybind_printDebugInfo       , DOKO_SCREEN_IMAGE, 0, DELAY_FAST},
    {KEY_SPACE            , keybind_moveImageByMouseDelta, DOKO_SCREEN_IMAGE, 0, DELAY_INSTANT},

#if (ENABLE_SHADERS == 1)
    {KEY_I, keybind_colorInvertShader   , DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
    {KEY_G, keybind_colorGrayscaleShader, DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
#else
    {KEY_I, keybind_colorInvert, DOKO_SCREEN_IMAGE, 0, DELAY_MEDIUM},
#endif


    // ###########################
    // ##### file list page ######
    // ###########################
    {KEY_K                            , keybind_prevImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_J                            , keybind_nextImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_K | SHIFT_MASK               , keybind_prevImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_J | SHIFT_MASK               , keybind_nextImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_D | CONTROL_MASK             , keybind_nextImageBy10 , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_U | CONTROL_MASK             , keybind_prevImageBy10 , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_D | CONTROL_MASK | SHIFT_MASK, keybind_nextImageBy10 , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_U | CONTROL_MASK | SHIFT_MASK, keybind_prevImageBy10 , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_P | CONTROL_MASK             , keybind_prevImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_N | CONTROL_MASK             , keybind_nextImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_FAST},
    {KEY_P | CONTROL_MASK | SHIFT_MASK, keybind_prevImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_N | CONTROL_MASK | SHIFT_MASK, keybind_nextImage     , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_G | SHIFT_MASK               , keybind_jumpImageEnd  , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {KEY_G                            , keybind_jumpImageStart, DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
};

// define mouse input mappings
static InputMapping mousebinds[] = {
    {MOUSE_WHEEL_FORWARD_BUTTON              , keybind_zoomInMousePosition  , DOKO_SCREEN_IMAGE, 0, DELAY_INSTANT},
    {MOUSE_WHEEL_BACKWARD_BUTTON             , keybind_zoomOutMousePosition , DOKO_SCREEN_IMAGE, 0, DELAY_INSTANT},
    {MOUSE_WHEEL_FORWARD_BUTTON  | SHIFT_MASK, keybind_nextImage            , DOKO_SCREEN_IMAGE, 0, DELAY_VFAST},
    {MOUSE_WHEEL_BACKWARD_BUTTON | SHIFT_MASK, keybind_prevImage            , DOKO_SCREEN_IMAGE, 0, DELAY_VFAST},
    {MOUSE_BUTTON_LEFT                       , keybind_moveImageByMouseDelta, DOKO_SCREEN_IMAGE, 0, DELAY_INSTANT},

    {MOUSE_WHEEL_FORWARD_BUTTON              , keybind_prevImage    , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {MOUSE_WHEEL_BACKWARD_BUTTON             , keybind_nextImage    , DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {MOUSE_WHEEL_FORWARD_BUTTON  | SHIFT_MASK, keybind_prevImageBy10, DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
    {MOUSE_WHEEL_BACKWARD_BUTTON | SHIFT_MASK, keybind_nextImageBy10, DOKO_SCREEN_FILE_LIST, 0, DELAY_INSTANT},
};




//
// Define resource
//
#ifdef DOKO_BUNDLE
    #ifdef RESOURCE_PATH
        #undef RESOURCE_PATH
    #endif
    #define RESOURCE_PATH "../"
#else
    // the directory which contains the 'resources' folder
    // when doing `make install` this is set to `/opt/doko/`
    // important that this always ends with a /
    #ifndef RESOURCE_PATH
        #define RESOURCE_PATH ""
    #endif
#endif

// path to unifont
#define UNIFONT_PATH (RESOURCE_PATH "resources/fonts/unifont-15.1.04.otf")

// the spacing for the above font
#define UNIFONT_SPACING 0

// the codepoints to load for the above font at startup
// if the font has utf8 you can put them here
#define CODEPOINT_INITIAL                                                      \
    " !\"#$%&'()*+,-./"                                                        \
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"                       \
    "abcdefghijklmnopqrstuvwxyz{|}"


// extension filter, finds these files when searching directory
#if(USE_IMLIB2)
    #define IMAGE_FILE_FILTER                                                      \
    ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm;.psd;.webp;.jxl;.avif;"     \
    ".tiff;.heif;"
#elif(USE_MAGICK_CONVERT)
    #define IMAGE_FILE_FILTER                                                      \
    ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm;.psd;.webp;.jxl;.avif;"     \
    ".tiff;.heif;"
#elif(USE_FFMPEG_CONVERT)
    #define IMAGE_FILE_FILTER                                                      \
    ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm;.psd;.webp;.jxl;.avif;"     \
    ".tiff;"
#else
    #define IMAGE_FILE_FILTER ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm;.psd;"
#endif








// 
// DO NOT CHANGE
//

#define ZOOM_LEVELS_SIZE (sizeof((ZOOM_LEVELS)) / sizeof((ZOOM_LEVELS[0])))

#define KEYBIND_COUNT (sizeof(keybinds) / sizeof(keybinds[0]))

#define MOUSEBIND_COUNT (sizeof(mousebinds) / sizeof(mousebinds[0]))


#endif
