

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

// define the height of the bar
#define INFO_BAR_HEIGHT 32
#define INFO_BAR_FONT_SIZE INFO_BAR_HEIGHT
#define INFO_BAR_LEFT_MARGIN 8

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
#define ENABLE_KEYBOARD_INPUT_NO_DELAY // keyboard input which ignore
                                       // KEY_TIME_LIMIT

#define ENABLE_MOUSE_INPUT             // all mouse input
#define ENABLE_FILE_DROP               // file drop into the app

// number of keybinds to interpret per render
#define KEY_LIMIT 3

// Time limit for triggering key presses (in seconds)
// Considering count key presses every KEY_TIME_LIMIT seconds using IsKeyDown.
#define KEY_TIME_LIMIT (1.0 / 9.0)

// number of mouses inputs to interpret per render
#define MOUSE_LIMIT 1

#define RAYLIB_QUIT_KEY KEY_Q

// define keybinds, you can find more methods in the keybind.h file
// define any new ones as you see fit
static const InputMapping keybinds[] = {
    {KEY_K, keybind_moveImageUp},
    {KEY_J, keybind_moveImageDown},
    {KEY_H, keybind_moveImageLeft},
    {KEY_L, keybind_moveImageRight},

    {KEY_K | SHIFT_MASK, keybind_zoomInCenterImage},
    {KEY_J | SHIFT_MASK, keybind_zoomOutCenterImage},
    {KEY_H | SHIFT_MASK, keybind_moveImageLeft},
    {KEY_L | SHIFT_MASK, keybind_moveImageRight},

    {KEY_H | CONTROL_MASK, keybind_PrevImage},
    {KEY_L | CONTROL_MASK, keybind_nextImage},

    {KEY_C, keybind_fitCenterImage},
    {KEY_I, keybind_colorInvert},

    {KEY_M, keybind_flipHorizontal},
    {KEY_V, keybind_flipVertical},

    {KEY_O, keybind_printDebugInfo},
};

// define keybinds which ignore KEY_TIME_LIMIT
static const InputMapping keybinds_no_time_limit[] = {
    {KEY_SPACE, keybind_moveImageByMouseDelta},
};

// use this to bind a mouse wheel scroll
#define MOUSE_WHEEL_FORWARD_BUTTON 666
#define MOUSE_WHEEL_BACKWARD_BUTTON 667

static const InputMapping mousebinds[] = {
    {MOUSE_WHEEL_FORWARD_BUTTON, keybind_zoomInMousePosition},
    {MOUSE_WHEEL_BACKWARD_BUTTON, keybind_zoomOutMousePosition},
    {MOUSE_WHEEL_FORWARD_BUTTON | SHIFT_MASK, keybind_nextImage},
    {MOUSE_WHEEL_BACKWARD_BUTTON | SHIFT_MASK, keybind_PrevImage},
    {MOUSE_BUTTON_LEFT, keybind_moveImageByMouseDelta},
};

#define KEYBIND_COUNT (sizeof(keybinds) / sizeof(keybinds[0]))
#define KEYBIND_NO_LIMIT_COUNT (sizeof(keybinds_no_time_limit) / sizeof(keybinds_no_time_limit[0]))
#define MOUSEBIND_COUNT (sizeof(mousebinds) / sizeof(mousebinds[0]))

#endif
