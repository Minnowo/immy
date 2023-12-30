


#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "config.h"
#include "darray.h"
#include "doko.h"


int hasInit = 0;

char* imageBufPath;
Texture2D imageBuf;
Texture2D backgroundBuf;

Color pixelGridColor;

Font unifont;

dint_arr_t font_codepoints;



void ui_loadUnifont() {

    L_D("Loading unifont from %s\n", UNIFONT_PATH);

    UnloadFont(unifont);
    unifont = LoadFontEx(UNIFONT_PATH, 32, font_codepoints.buffer,
                         font_codepoints.size);
}

void ui_setInitialCodePoints(const char* text) {

    int codep_count;
    int *codep = LoadCodepoints(text, &codep_count);

    for (int c = 0; c < codep_count; c++) {
        DARRAY_APPEND(font_codepoints, codep[c]);
    }

    UnloadCodepoints(codep);
    ui_loadUnifont();
}

void ui_loadCodepoints(const char* text, bool reload) {

    int codep_count;
    int *codep;

    int BAD_C = GetGlyphIndex(unifont, '?');

    codep = LoadCodepoints(text, &codep_count);

    for (int c = 0; c < codep_count; c++) {

        int g = GetGlyphIndex(unifont, codep[c]);

        if (g == BAD_C && codep[c] != '?') {

            L_D("Adding new codepoint %d", codep[c]);

            DARRAY_APPEND(font_codepoints, codep[c]);
        }
    }

    UnloadCodepoints(codep);

    if (reload) {
        ui_loadUnifont();
    }
}


void ui_loadCodepointsFromFileList(doko_control_t* ctrl){

    DARRAY_FOR_EACH_I(ctrl->image_files, i) {

        doko_image_t *im = ctrl->image_files.buffer + i;

        ui_loadCodepoints(im->path + im->nameOffset, false);
    }

    ui_loadUnifont();
}


void ui_init() {

    InitWindow(START_WIDTH, START_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(WINDOW_FPS);
    SetExitKey(RAYLIB_QUIT_KEY);

    backgroundBuf = ui_loadBackgroundTile(BACKGROUND_TILE_W, BACKGROUND_TILE_H,
                                       (Color)BACKGROUND_TILE_COLOR_A_RGBA,
                                       (Color)BACKGROUND_TILE_COLOR_B_RGBA);

    DARRAY_INIT(font_codepoints, 128);
    ui_setInitialCodePoints(CODEPOINT_INITIAL);

    pixelGridColor = (Color)PIXEL_GRID_COLOR_RGBA;

    imageBuf.id = 0;
    imageBufPath = NULL;
    hasInit = 1;
}

void ui_deinit() {

    if(!hasInit) {
        exit(1);
        return;
    }

    UnloadTexture(imageBuf);
    UnloadTexture(backgroundBuf);
    UnloadFont(unifont);

    CloseWindow();

    hasInit = 0;
}


Texture2D ui_loadBackgroundTile(size_t w, size_t h, Color a, Color b) {

    Image image = GenImageColor(w, h, a);
    ImageDrawRectangle(&image, 0, 0, w / 2, h / 2, b);
    ImageDrawRectangle(&image, w / 2, h / 2, w / 2, h / 2, b);

    Texture2D t = LoadTextureFromImage(image);

    UnloadImage(image);

    return t;
}

void ui_renderTextOnInfoBar(const char* text) {

    int sw = GetScreenWidth() ;
    int sh = ImageViewHeight;

    int fontSize = unifont.baseSize;

    Vector2 textSize;

    do {
        textSize = MeasureTextEx(unifont, text, fontSize, UNIFONT_SPACING);
    } while (textSize.x > sw - INFO_BAR_LEFT_MARGIN && --fontSize);

    DrawRectangle(0, sh, sw,  INFO_BAR_HEIGHT, BLACK);

    DrawTextEx(unifont, text, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, fontSize, UNIFONT_SPACING,
               WHITE);
}

void ui_renderBackground() {

    DrawTextureRec(backgroundBuf,
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0}, WHITE);
}

void ui_renderImage(doko_image_t* image) {

    if(image->status == IMAGE_STATUS_FAILED) {
        return;
    }

    if (imageBufPath != image->path) {

        if (image->status == IMAGE_STATUS_NOT_LOADED) {
            if (!doko_loadImage(image)) {
                return;
            }
        }

        Texture2D nimageBuf = LoadTextureFromImage(image->rayim);

        if(nimageBuf.id == 0) {
            return;
        }

        if(imageBuf.id > 0) {
            UnloadTexture(imageBuf);
        }

        imageBufPath = image->path;
        imageBuf = nimageBuf;
    }
    else if (image->rebuildBuff) {

        image->rebuildBuff = 0;
        UpdateTexture(imageBuf,image->rayim.data);
    }

    DrawTextureEx(imageBuf, image->dstPos, image->rotation, image->scale,
                  WHITE);
}

void ui_renderInfoBar(doko_image_t *image) {
    // ui_renderTextOnInfoBar(TextFormat(
    //     "%0.0f x %0.0f  %0.0f%%  %s", image->srcRect.width, image->srcRect.height,
    //     image->scale*100, image->path + image->nameOffset));

    const char *prefix = TextFormat("%0.0f x %0.0f  %0.0f%%  ", image->srcRect.width,
                              image->srcRect.height, image->scale * 100);

    char* postfix = image->path + image->nameOffset;

    int sw = GetScreenWidth() ;
    int sh = ImageViewHeight;

    int fontSize = unifont.baseSize;

    Vector2 pretextSize;
    Vector2 textSize;

    pretextSize = MeasureTextEx(unifont, prefix, fontSize, UNIFONT_SPACING);

    do {
        textSize = MeasureTextEx(unifont, postfix, fontSize, UNIFONT_SPACING);
    } while (textSize.x > (sw - pretextSize.x) &&
             --fontSize);

    DrawRectangle(0, sh, sw,  INFO_BAR_HEIGHT, BLACK);

    DrawTextEx(unifont, prefix, (Vector2){INFO_BAR_LEFT_MARGIN, sh},
               unifont.baseSize, UNIFONT_SPACING, WHITE);

    DrawTextEx(unifont, postfix,
               (Vector2){ pretextSize.x,
                         sh + (INFO_BAR_HEIGHT - fontSize) / 2.0},
               fontSize, UNIFONT_SPACING, WHITE);
}

void ui_renderPixelGrid(doko_image_t* image) {

    if (image->scale < SHOW_PIXEL_GRID_SCALE_THRESHOLD) {
        return;
    }

    float x = image->dstPos.y;
    float y = image->dstPos.x;
    float w = ImageViewWidth;
    float h = ImageViewHeight;

    for (float i = x, j = y; i <= h || j <= w;
         i += image->scale, j += image->scale) {
        DrawLine(0, i, w, i, pixelGridColor);
        DrawLine(j, 0, j, h, pixelGridColor);
    }

    for (float i = x, j = y; i > 0 || j > 0;
         i -= image->scale, j -= image->scale) {
        DrawLine(0, i, w, i, pixelGridColor);
        DrawLine(j, 0, j, h, pixelGridColor);
    }
}


void ui_renderFileList(doko_control_t* ctrl) {

    #define FZ unifont.baseSize
    #define BOTTOM_MARGIN 1

    int sw = GetScreenWidth() ;
    int sh = GetScreenHeight() - (GetScreenHeight() / 8);

    int startY; 
    int scrollOffset; ;
    int startIndex = 0;

    scrollOffset = (sh / FZ) / 2;

    if (scrollOffset < ctrl->selected_index) {
        startIndex = ctrl->selected_index - scrollOffset;

        if (startIndex + scrollOffset * 2 > ctrl->image_files.size) {
            startIndex = ctrl->image_files.size - scrollOffset * 2;
        }
    }

    startY = GetScreenHeight() -
             MIN(ctrl->image_files.size * FZ, (scrollOffset * 2) * FZ) -
             BOTTOM_MARGIN * FZ;

    size_t i = startIndex;
    DARRAY_FOR_I(ctrl->image_files, i, startIndex+scrollOffset*2) {

        doko_image_t *im = ctrl->image_files.buffer + i;

        int y = startY + FZ * (i-startIndex);

        if (ctrl->selected_image == NULL || i != ctrl->selected_index) {
            DrawRectangle(0, y, sw, FZ, BLACK);
        } else {
            DrawRectangle(0, y, sw, FZ, GREEN);
        }

        DrawTextEx(unifont,TextFormat("%02d  %s",i, im->path + im->nameOffset), (Vector2){FILE_LIST_LEFT_MARGIN, y}, FZ,UNIFONT_SPACING, WHITE);
    }

    DrawRectangle(0, startY + FZ * (i - startIndex), sw, FZ, BLACK);
    DrawTextEx(unifont,
               TextFormat("%d more files...", ctrl->image_files.size - i),
               (Vector2){FILE_LIST_LEFT_MARGIN, startY + FZ * (i - startIndex)},
               FZ, UNIFONT_SPACING, WHITE);
}


