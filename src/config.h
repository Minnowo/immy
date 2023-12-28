

#ifndef DOKO_CONFIG_H
#define DOKO_CONFIG_H

#include <raylib.h>
#include <stddef.h>

#include "doko.h"
#include "input.h"

#define WINDOW_TITLE "Doko?"

#define WINDOW_FPS 24

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

#define START_WIDTH 512
#define START_HEIGHT 512

#define MIN_WINDOW_WIDTH 64
#define MIN_WINDOW_HEIGHT 64

// Size of the tile pattern
#define BACKGROUND_TILE_W 128
#define BACKGROUND_TILE_H 128

// Background colors for the tile pattern, (r, g, b, a)
#define BACKGROUND_TILE_COLOR_A_RGBA                                           \
    { 32, 32, 32, 255 }
#define BACKGROUND_TILE_COLOR_B_RGBA                                           \
    { 64, 64, 64, 255 }


// When the image is off-screen, try to keep at least IMAGE_INVERSE_MARGIN
// visible on the screen. `x` is for the horizontal (left/right of the screen).
// `y` is for the vertical (top/bottom of the screen).
#define IMAGE_INVERSE_MARGIN_X 32
#define IMAGE_INVERSE_MARGIN_Y 32


// the relative path to unifont
#define UNIFONT_PATH "resources/fonts/unifont-15.1.04.otf"
#define UNIFONT_SPACING 0

// the codepoints to load for the above font at startup
#define CODEPOINT_INITIAL                                                      \
    " !\"#$%&'()*+,-./"                                                        \
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"                       \
    "abcdefghijklmnopqrstuvwxyz{|}"

// define the height of the bar
#define INFO_BAR_HEIGHT 32
#define INFO_BAR_FONT_SIZE INFO_BAR_HEIGHT
#define INFO_BAR_LEFT_MARGIN 8

#define FILE_LIST_FONT_SIZE 24
#define FILE_LIST_LEFT_MARGIN INFO_BAR_LEFT_MARGIN

// show the pixel grid when the scale is bigger than this value
#define SHOW_PIXEL_GRID_SCALE_THRESHOLD 20
#define PIXEL_GRID_COLOR_RGBA                                                  \
    { 200, 200, 200, 200 }

// See the top of doko.c, where this array is defined.
// It MUST be sorted.
extern const double ZOOM_LEVELS[];

// the smallest scale value in the ZOOM_LEVELS array
#define SMALLEST_SCALE_VALUE 0.01

// 1 searches directories recursivly
// 0 does not search directories recursivly
#define SEARCH_DIRS_RECURSIVE 1

// extension filter, finds these files when searching directory
#define IMAGE_FILE_FILTER ".png;.jpg;.jpeg;.bmp;.gif;.tga;.hdr;.ppm;.pgm"


#define ENABLE_KEYBOARD_INPUT          // all keyboard input
#define ENABLE_MOUSE_INPUT             // all mouse input
#define ENABLE_FILE_DROP               // file drop into the app

// number of keybinds to interpret per render
#define KEY_LIMIT 3


// number of mouses inputs to interpret per render
#define MOUSE_LIMIT 1

#define RAYLIB_QUIT_KEY KEY_Q

// Time limit for triggering key presses (in seconds)
#define _200MS (0.2)
#define _110MS (0.11111)
#define _80MS (0.08)

// define keybinds, you can find more methods in the keybind.h file
// define any new ones as you see fit
static InputMapping keybinds[] = {

    // KEY,        KEY_CALLBACK,          SCREEN,     LAST_PRESSED,        KEY_TRIGGER_RATE

    // image view
    {KEY_K, keybind_moveImageUp, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_J, keybind_moveImageDown, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_H, keybind_moveImageLeft, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_L, keybind_moveImageRight, DOKO_SCREEN_IMAGE, 0, _110MS},

    {KEY_K | SHIFT_MASK, keybind_zoomInCenterImage, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_J | SHIFT_MASK, keybind_zoomOutCenterImage, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_H | SHIFT_MASK, keybind_moveImageLeft, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_L | SHIFT_MASK, keybind_moveImageRight, DOKO_SCREEN_IMAGE, 0, _110MS},

    {KEY_H | CONTROL_MASK, keybind_PrevImage, DOKO_SCREEN_IMAGE, 0, _200MS},
    {KEY_L | CONTROL_MASK, keybind_nextImage, DOKO_SCREEN_IMAGE, 0, _200MS},

    {KEY_C, keybind_fitCenterImage, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_I, keybind_colorInvert, DOKO_SCREEN_IMAGE, 0, _110MS},

    {KEY_M, keybind_flipHorizontal, DOKO_SCREEN_IMAGE, 0, _110MS},
    {KEY_V, keybind_flipVertical, DOKO_SCREEN_IMAGE, 0, _110MS},

    {KEY_O, keybind_printDebugInfo, DOKO_SCREEN_IMAGE, 0, _110MS},

    {KEY_SPACE, keybind_moveImageByMouseDelta, DOKO_SCREEN_IMAGE, 0, 0},

    // file list
    {KEY_K, keybind_PrevImage, DOKO_SCREEN_FILE_LIST, 0, _80MS},
    {KEY_J, keybind_nextImage, DOKO_SCREEN_FILE_LIST, 0, _80MS},

};

// use this to bind a mouse wheel scroll
#define MOUSE_WHEEL_FORWARD_BUTTON 666
#define MOUSE_WHEEL_BACKWARD_BUTTON 667

static InputMapping mousebinds[] = {
    {MOUSE_WHEEL_FORWARD_BUTTON, keybind_zoomInMousePosition, DOKO_SCREEN_IMAGE, 0, 0},
    {MOUSE_WHEEL_BACKWARD_BUTTON, keybind_zoomOutMousePosition, DOKO_SCREEN_IMAGE, 0, 0},
    {MOUSE_WHEEL_FORWARD_BUTTON | SHIFT_MASK, keybind_nextImage, DOKO_SCREEN_IMAGE, 0, _80MS},
    {MOUSE_WHEEL_BACKWARD_BUTTON | SHIFT_MASK, keybind_PrevImage, DOKO_SCREEN_IMAGE, 0, _80MS},
    {MOUSE_BUTTON_LEFT, keybind_moveImageByMouseDelta, DOKO_SCREEN_IMAGE, 0, 0},
};

#define KEYBIND_COUNT (sizeof(keybinds) / sizeof(keybinds[0]))
#define MOUSEBIND_COUNT (sizeof(mousebinds) / sizeof(mousebinds[0]))

#endif
