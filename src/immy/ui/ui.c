
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../darray.h"
#include "../core/core.h"
#include "../resources.h"
#include "ui.h"

bool       g_uiReady         = false; // is the ui ready for drawing
Font       g_unifont         = {0};   // global font for ui text
int        g_unifontSize     = 32;    // font size
dIntArr_t  g_fontCodepoints  = {0};   // codepoints for ui font

Color     g_pixelGridColor = PIXEL_GRID_COLOR_RGBA; // the pixel grid color
Texture2D g_backgroundBuf  = {0};                   // the background texture

#if ENABLE_SHADERS

Shader grayscaleShader              = {0};
bool   applyInvertShaderValue       = false;
bool   applyGrayscaleShaderValue    = false;
int    grayInvertEffectLocation     = 0;

#endif

void uiInit(ImmyConfig_t* config) {

    // prevents glfw from hanging when in fullscreen on X11
    SetConfigFlags(FLAG_WINDOW_HIDDEN);

    InitWindow(config->window_width, 
               config->window_height, 
               config->window_title);

    if (config->x_override_redirect) {

        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    } else {
        // This breaks override redirect?????
        ClearWindowState(FLAG_WINDOW_HIDDEN);
    }

    SetWindowMinSize(config->window_min_width, 
                     config->window_min_height);

    SetWindowState(config->window_flags);

    if (config->set_win_position)
        SetWindowPosition(config->window_x, 
                          config->window_y);

    SetTargetFPS(WINDOW_FPS);
    SetExitKey(RAYLIB_QUIT_KEY);

    g_backgroundBuf = uiLoadBackgroundTile(
        BACKGROUND_TILE_W, 
        BACKGROUND_TILE_H,
        BACKGROUND_TILE_COLOR_A_RGBA, 
        BACKGROUND_TILE_COLOR_B_RGBA
    );

#if ENABLE_SHADERS
    grayscaleShader = LoadShaderFromMemory(NULL, INVERT_AND_GRAYSCALE_SHADER_CODE);
    grayInvertEffectLocation = GetShaderLocation(grayscaleShader, "effects");
#endif

    dIntArrInit(&g_fontCodepoints, 128);

    uiSetInitialCodePoints(CODEPOINT_INITIAL);

    g_pixelGridColor = PIXEL_GRID_COLOR_RGBA;
    g_uiReady        = 1;
}

void uiDeinit() {

    if (!g_uiReady) {

        DIE("Cannot deinit if we have no init the ui");
        return;
    }

    uiThumbPageClearState();

    uiImagePageClearState();

    UnloadTexture(g_backgroundBuf);

    UnloadFont(g_unifont);

    dIntArrFree(&g_fontCodepoints);

#if ENABLE_SHADERS
    UnloadShader(grayscaleShader);
#endif

    CloseWindow();

    g_uiReady = false;
}

void uiLoadUnifont() {

    L_D("Loading g_uifont from %s\n", UNIFONT_PATH);

    size_t size;

    unsigned char* data = get_resource_data(UNIFONT_PATH, &size);

    if (!data) {

        L_E("Cannot load g_uifont %s!", UNIFONT_PATH);

        return;
    }

    UnloadFont(g_unifont);

    g_unifont = LoadFontFromMemory(
        GetFileExtension(UNIFONT_PATH), data, size, g_unifontSize,
        g_fontCodepoints.buffer, g_fontCodepoints.size
    );

    /* force raygui font
    GuiSetFont(g_unifont);
    for(int i = 0; i < STATUSBAR; i++)
        GuiSetStyle(i, TEXT_SIZE, g_unifontSize);
        */

    free_resource_data((void*)data);
}

void uiSetInitialCodePoints(const char* text) {

    int  codep_count;
    int* codep = LoadCodepoints(text, &codep_count);

    for (int c = 0; c < codep_count; c++) {
        dIntArrAppend(&g_fontCodepoints, codep[c]);
    }

    UnloadCodepoints(codep);
    uiLoadUnifont();
}

void uiLoadCodepoints(const char* text, bool reload) {

    L_D("Loading codepoints for text: %s", text);

    int  codep_count;
    int  BAD_C = GetGlyphIndex(g_unifont, '?');
    int* codep = LoadCodepoints(text, &codep_count);

    if (codep == NULL || codep_count == 0)
        return;

    for (int c = 0; c < codep_count; c++) {

        int g = GetGlyphIndex(g_unifont, codep[c]);

        if (g == BAD_C && codep[c] != '?') {

            L_D("Adding new codepoint %d", codep[c]);

            dIntArrAppend(&g_fontCodepoints, codep[c]);
        }
    }

    UnloadCodepoints(codep);

    if (reload) {
        uiLoadUnifont();
    }
}

void uiLoadCodepointsFromFileList(const ImmyControl_t* ctrl) {

    DARRAY_FOR_EACH(ctrl->image_files, i) {

        ImmyImage_t* im = ctrl->image_files.buffer + i;

        uiLoadCodepoints(im->path, false);
    }

    uiLoadUnifont();
}



Texture2D uiLoadBackgroundTile(size_t w, size_t h, Color a, Color b) {

    Image image = GenImageColor(w, h, a);
    ImageDrawRectangle(&image, 0, 0, w / 2, h / 2, b);
    ImageDrawRectangle(&image, w / 2, h / 2, w / 2, h / 2, b);

    Texture2D t = LoadTextureFromImage(image);

    UnloadImage(image);

    return t;
}

void uiRenderBackground() {

    DrawTextureRec(
        g_backgroundBuf, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, WHITE
    );
}




