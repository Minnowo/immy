

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

// if 1, enable detaching from terminal
#define ALLOW_DETACH_FROM_TERMINAL 1

// default value for detaching from terminal
#define DETACH_FROM_TERMINAL 1

// use imlib2
// you will also need to set -lImlib2 in the LDFLAGS of the makefile
#define USE_IMLIB2 1

// if 1 load images using image magick convert before raylib
// requires image magick's `convert` added to path
#define USE_MAGICK 1
// have magick convert to this format before raylib loads the image
#define MAGICK_CONVERT_MIDDLE_FMT "qoi"

// if 1 try and load images using ffmpeg before raylib
// requires `ffmpeg` added to path
#define USE_FFMPEG 1
// have ffmpeg convert to this format before raylib loads the image
#define FFMPEG_CONVERT_MIDDLE_FMT "qoi"
// see https://ffmpeg.org/ffmpeg.html
// this is the value set for the -loglevel and -v flags
#define FFMPEG_VERBOSITY "error"

// if 1, load images in separate threads
#define ASYNC_IMAGE_LOADING 1

// the command which is run to copy images to clipobard for X11
// the command must accept png bytes through stdin
#define X11_COPY_IMAGE_COMMAND "xclip -selection clipboard -target image/png"


// window sizes
#define START_WIDTH 512
#define START_HEIGHT 512
#define MIN_WINDOW_WIDTH 64
#define MIN_WINDOW_HEIGHT 64

#define DEFAULT_SORT_ORDER SORT_ORDER__NATURAL

// if 1, center the first image on start
#define CENTER_IMAGE_ON_FIRST_START 1

// Size of the background tile pattern
#define BACKGROUND_TILE_W 128
#define BACKGROUND_TILE_H 128

// always try and keep this many pixels of the image on screen
#define IMAGE_INVERSE_MARGIN_X 32
#define IMAGE_INVERSE_MARGIN_Y 32

#define THUMBNAIL_SIZE 256

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
#define DELAY_MEDIUM  (0.17) /*(0.2)*/
#define DELAY_FAST    (0.120)
#define DELAY_VFAST   (0.08)
#define DELAY_ALMOST_INSTANT (0.03)
#define DELAY_INSTANT (0)


// To find all the keys you can bind see
//  raylib/raylib-5.0/src/raylib.h about line 550 for keys

// Since raylib doesn't have a key for the scroll wheel delta
// Use these to bind a mouse wheel scroll
typedef enum { 
    MOUSE_WHEEL_FWD = 666,
    MOUSE_WHEEL_BWD = 667
} MouseWheel;

// define keybinds
// define any new ones as you see fit
static InputMapping keybinds[] = {

    // KEY,        KEY_CALLBACK,          SCREEN,     LAST_PRESSED,  KEY_TRIGGER_RATE

    // ###########################
    // ##### all pages ###########
    // ###########################
    BIND(KEY_T    , kb_Toggle_Image_Filelist_Screen, DOKO_SCREEN__ALL, DELAY_MEDIUM ),
    BIND(KEY_ONE  , kb_Goto_Image_Screen           , DOKO_SCREEN__ALL, DELAY_MEDIUM ),
    BIND(KEY_TWO  , kb_Goto_File_List_Screen       , DOKO_SCREEN__ALL, DELAY_MEDIUM ),
    BIND(KEY_THREE, kb_Goto_Thumb_Screen           , DOKO_SCREEN__ALL, DELAY_MEDIUM ),
    BIND(KEY_FOUR , kb_Goto_Keybinds_Screen        , DOKO_SCREEN__ALL, DELAY_MEDIUM ),

    // ###########################
    // ##### image view page #####
    // ###########################
    BIND(KEY_K                , kb_Move_Image_Up            , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_J                , kb_Move_Image_Down          , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H                , kb_Move_Image_Left          , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_L                , kb_Move_Image_Right         , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H | SHIFT_MASK   , kb_Move_Image_Left          , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_L | SHIFT_MASK   , kb_Move_Image_Right         , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_K | SHIFT_MASK   , kb_Zoom_In_Center_Image     , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_J | SHIFT_MASK   , kb_Zoom_Out_Center_Image    , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_S                , kb_Zoom_In_Center_Image     , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_D                , kb_Zoom_Out_Center_Image    , DOKO_SCREEN_IMAGE, DELAY_FAST),
    BIND(KEY_H | CONTROL_MASK , kb_Prev_Image               , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_L | CONTROL_MASK , kb_Next_Image               , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_P                , kb_Prev_Image               , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_N                , kb_Next_Image               , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_C | CONTROL_MASK , kb_Copy_Image_To_Clipboard  , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_C                , kb_Fit_Center_Image         , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_M                , kb_Flip_Horizontal          , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_V                , kb_Flip_Vertical            , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_B                , kb_Toggle_Show_Bar          , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_O                , kb_Print_Debug_Info         , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_SPACE            , kb_Move_Image_By_Mouse_Delta, DOKO_SCREEN_IMAGE, DELAY_INSTANT),

#if (ENABLE_SHADERS == 1)
    BIND(KEY_I, kb_Color_Invert_Shader   , DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
    BIND(KEY_G, kb_Color_Grayscale_Shader, DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
#else
    BIND(KEY_I, keybind_colorInvert, DOKO_SCREEN_IMAGE, DELAY_MEDIUM),
#endif


    // ###########################
    // ##### file list page ######
    // ###########################
    BIND(KEY_K                            , kb_Prev_Image      , DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_J                            , kb_Next_Image      , DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_K | SHIFT_MASK               , kb_Prev_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_J | SHIFT_MASK               , kb_Next_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_D | CONTROL_MASK             , kb_Next_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_U | CONTROL_MASK             , kb_Prev_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_D | CONTROL_MASK | SHIFT_MASK, kb_Next_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_U | CONTROL_MASK | SHIFT_MASK, kb_Prev_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_P | CONTROL_MASK             , kb_Prev_Image      , DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_N | CONTROL_MASK             , kb_Next_Image      , DOKO_SCREEN_FILE_LIST, DELAY_FAST),
    BIND(KEY_P | CONTROL_MASK | SHIFT_MASK, kb_Prev_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_N | CONTROL_MASK | SHIFT_MASK, kb_Next_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_G | SHIFT_MASK               , kb_Jump_Image_End  , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(KEY_G                            , kb_Jump_Image_Start, DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),


    // ###########################
    // ##### thumbs    page ######
    // ###########################
    BIND(KEY_K, kb_Scroll_Thumb_Page_Up  , DOKO_SCREEN_THUMB_GRID, DELAY_FAST),
    BIND(KEY_K, kb_Scroll_Thumb_Page_Up  , DOKO_SCREEN_THUMB_GRID, DELAY_FAST),

    // ###########################
    // ##### keybind   page ######
    // ###########################
    BIND(KEY_K, kb_Scroll_Keybind_List_Up  , DOKO_SCREEN_KEYBINDS, DELAY_FAST),
    BIND(KEY_J, kb_Scroll_Keybind_List_Down, DOKO_SCREEN_KEYBINDS, DELAY_FAST),
};

// define mouse input mappings
static InputMapping mousebinds[] = {
    BIND(MOUSE_WHEEL_FWD             , kb_Zoom_In_Mouse_Position   , DOKO_SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD             , kb_Zoom_Out_Mouse_Position  , DOKO_SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_FWD | SHIFT_MASK, kb_Next_Image               , DOKO_SCREEN_IMAGE, DELAY_VFAST),
    BIND(MOUSE_WHEEL_BWD | SHIFT_MASK, kb_Prev_Image               , DOKO_SCREEN_IMAGE, DELAY_VFAST),
    BIND(MOUSE_BUTTON_LEFT           , kb_Move_Image_By_Mouse_Delta, DOKO_SCREEN_IMAGE, DELAY_INSTANT),
    BIND(MOUSE_BUTTON_RIGHT          , kb_Move_Image_By_Mouse_Delta, DOKO_SCREEN_IMAGE, DELAY_INSTANT),

    BIND(MOUSE_WHEEL_FWD             , kb_Prev_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD             , kb_Next_Image      , DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_FWD | SHIFT_MASK, kb_Prev_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD | SHIFT_MASK, kb_Next_Image_By_10, DOKO_SCREEN_FILE_LIST, DELAY_INSTANT),

    BIND(MOUSE_WHEEL_FWD, kb_Scroll_Keybind_List_Up  , DOKO_SCREEN_KEYBINDS, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD, kb_Scroll_Keybind_List_Down, DOKO_SCREEN_KEYBINDS, DELAY_INSTANT),

    BIND(MOUSE_WHEEL_FWD, kb_Scroll_Thumb_Page_Up  , DOKO_SCREEN_THUMB_GRID, DELAY_INSTANT),
    BIND(MOUSE_WHEEL_BWD, kb_Scroll_Thumb_Page_Down, DOKO_SCREEN_THUMB_GRID, DELAY_INSTANT),
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
    "Usage: doko [options] filename/dirname\n"                                 \
    "\n"                                                                       \
    "Basic optionsa:"                                                          \
    "\n  " CLI_HELP_f_FLAG "\n  " CLI_HELP_B_FLAG "\n  " CLI_HELP_b_FLAG       \
    "\n  " CLI_HELP_D_FLAG "\n  " CLI_HELP_d_FLAG "\n  " CLI_HELP_C_FLAG       \
    "\n  " CLI_HELP_c_FLAG "\n  " CLI_HELP_t_FLAG "\n  " CLI_HELP_x_FLAG       \
    "\n  " CLI_HELP_y_FLAG "\n  " CLI_HELP_w_FLAG "\n  " CLI_HELP_h_FLAG       \
    "\n  " CLI_HELP_l_FLAG

// 
// DO NOT CHANGE
//

#define ZOOM_LEVELS_SIZE (sizeof((ZOOM_LEVELS)) / sizeof((ZOOM_LEVELS[0])))

#define KEYBIND_COUNT (sizeof(keybinds) / sizeof(keybinds[0]))

#define MOUSEBIND_COUNT (sizeof(mousebinds) / sizeof(mousebinds[0]))


#endif
