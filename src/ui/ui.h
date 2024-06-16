

#ifndef IMMY_UI_H
#define IMMY_UI_H

#include <raylib.h>
#include <stddef.h>

#include "../config.h"
#include "../core/core.h"

extern bool       g_uiReady;        // is the ui ready for drawing
extern Font       g_unifont;         // global font for ui text
extern int        g_unifontSize;     // font size
extern dint_arr_t g_fontCodepoints; // codepoints for ui font

extern Color     g_pixelGridColor; // the pixel grid color
extern Texture2D g_backgroundBuf;  // the background texture

#if ENABLE_SHADERS

// just for the one shader
extern Shader grayscaleShader;
extern bool   applyInvertShaderValue;
extern bool   applyGrayscaleShaderValue;
extern int    invertShaderValueLocation;
extern int    grayscaleShaderValueLocation;

#endif

/* for when we start using raygui, or continue with our own gui
#if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
    #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT INFO_BAR_HEIGHT
#endif

#if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
    #define RAYGUI_WINDOW_CLOSEBUTTON_SIZE (INFO_BAR_HEIGHT-6)
#endif

// panel definitions
typedef enum PanelUpdateResult {
    PUR_NONE,
    PUR_BLOCK_INPUT,
    PUR_BLOCK_MOUSE_INPUT,
    PUR_BLOCK_KEYBOARD_INPUT,
} PanelUpdateResult_t;

typedef struct BasePanel {
    Rectangle rect;
    Vector2 dragOffset;
    bool dragging;
} BasePanel_t;

typedef struct DraggablePanel {
    Rectangle rect;
    Vector2 dragOffset;
    bool dragging;
} DraggablePanel_t;

*/


// helper functions
static inline void uiDrawTextAt(const char* str, float x, float y) {
    return DrawTextEx(
        g_unifont, str, (Vector2){x, y}, g_unifont.baseSize, 1, TEXT_COLOR_RGBA
    );
}

static inline void uiDrawText(const char* str) {
    return uiDrawTextAt(str, 0, 0);
}

static inline Color uiColorInvert(Color c) {
    c.r = 255 - c.r;
    c.g = 255 - c.g;
    c.b = 255 - c.b;
    return c;
}


void uiInit(ImmyConfig_t* ctrl); // init the ui
void uiDeinit();                  // deinit the ui

// font functions
void uiLoadUnifont();                                          // unload and load unifont
void uiSetInitialCodePoints(const char* text);                 // load these codepoints
void uiLoadCodepoints(const char* text, bool reloadFont);      // load codepoints from
void uiLoadCodepointsFromFileList(const ImmyControl_t* ctrl); // load codepoints from

// background functions
Texture2D uiLoadBackgroundTile(size_t w, size_t h, Color a, Color b); // get the background texture
void      uiRenderBackground();                                       // render the background

// image screen functions
void uiImagePageClearState();                                                    // clear any state
void uiRenderImage(ImmyImage_t* im);                                            // draw the image
void uiRenderPixelGrid(const ImmyImage_t* image);                               // draw a pixel grid
void uiRenderTextOnInfoBar(const char* text);                                    // draw text onto the bar
void uiRenderInfoBar(const ImmyImage_t* image);                                 // draw image info on the bar
void uiFitCenterImage(ImmyImage_t* image);                                      // fit the image & center the image
void uiCenterImage(ImmyImage_t* image);                                         // center the image at current size
void uiEnsureImageNotLost(ImmyImage_t* image);                                  // makes sure the image is partially on screen
void uiMoveScrFracImage(ImmyImage_t* im, double xFrac, double yFrac);           // moves image by screen frac
void uiZoomImageCenter(ImmyImage_t* im, double afterZoom);                      // zoom in on center
void uiZoomImageOnPoint(ImmyImage_t* im, double afterZoom, int x, int y);       // zoom in on point
void uiZoomImageCenterFromClosest(ImmyImage_t* im, bool zoomIn);                // zoom in on center but lock to ZOOM_LEVELS
void uiZoomImageOnPointFromClosest(ImmyImage_t* im, bool zoomIn, int x, int y); // zoom in on point but lock to ZOOM_LEVELS
Rectangle uiGetScreenPadding();                                                  // get the image viewport
void uiSetScreenPadding(Rectangle viewPort);                                     // set the image viewport
void uiSetScreenPaddingRight(float width);                                       // set right image padding
void uiSetScreenPaddingBottom(float height);                                     // set bottom image padding
void uiSetScreenPaddingLeft(float x);                                            // set left image padding
void uiSetScreenPaddingTop(float y);                                             // set left image padding

// thumbnail screen functions
void uiThumbPageClearState();                    // clear any state
void uiRenderThumbs(const ImmyControl_t* ctrl); // render thumbnails

// file list screen functions
void uiRenderFileList(const ImmyControl_t* ctrl);

// keybinds screen functions
void uiRenderKeybinds(const ImmyControl_t* ctrl);


/*
// test panel functions
PanelUpdateResult_t updateTestPanel(DraggablePanel_t* panel);
void                drawTestPanel(const DraggablePanel_t* panel);
*/

#endif
