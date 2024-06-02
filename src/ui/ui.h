

#ifndef DOKO_UI_H
#define DOKO_UI_H

#include <raylib.h>
#include <stddef.h>

#include "../config.h"
#include "../doko.h"

extern bool       g_uiReady;        // is the ui ready for drawing
extern Font       g_unifont;         // global font for ui text
extern int        g_unifontSize;     // font size
extern dint_arr_t g_fontCodepoints; // codepoints for ui font

extern Color     g_pixelGridColor; // the pixel grid color
extern Texture2D g_backgroundBuf;  // the background texture

#if (ENABLE_SHADERS == 1)

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


void uiInit(doko_config_t* ctrl); // init the ui
void uiDeinit();                  // deinit the ui

// font functions
void uiLoadUnifont();                                          // unload and load unifont
void uiSetInitialCodePoints(const char* text);                 // load these codepoints
void uiLoadCodepoints(const char* text, bool reloadFont);      // load codepoints from
void uiLoadCodepointsFromFileList(const doko_control_t* ctrl); // load codepoints from

// background functions
Texture2D uiLoadBackgroundTile(size_t w, size_t h, Color a, Color b); // get the background texture
void      uiRenderBackground();                                       // render the background

// image screen functions
void uiImagePageClearState();                      // clear any state
void uiRenderImage(doko_image_t* im);              // draw the image
void uiRenderPixelGrid(const doko_image_t* image); // draw a pixel grid
void uiRenderTextOnInfoBar(const char* text);      // draw text onto the bar
void uiRenderInfoBar(const doko_image_t* image);   // draw image info on the bar

// thumbnail screen functions
void uiThumbPageClearState();                    // clear any state
void uiRenderThumbs(const doko_control_t* ctrl); // render thumbnails

// file list screen functions
void uiRenderFileList(const doko_control_t* ctrl);

// keybinds screen functions
void uiRenderKeybinds(const doko_control_t* ctrl);


/*
// test panel functions
PanelUpdateResult_t updateTestPanel(DraggablePanel_t* panel);
void                drawTestPanel(const DraggablePanel_t* panel);
*/

#endif
