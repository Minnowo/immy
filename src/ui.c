
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "darray.h"
#include "doko.h"
#include "resources.h"
#include "ui.h"

int hasInit = 0;

char*     imageBufPath;
Texture2D imageBuf;
Texture2D backgroundBuf;
Texture2D_dynamic_arr_t thumbBufs;

#if (ENABLE_SHADERS == 1)
Shader grayscaleShader;
bool   applyInvertShaderValue    = 0;
bool   applyGrayscaleShaderValue = 1;
int    applyInvertShaderValueLocation;
int    applyGrayscaleShaderValueLocation;
#endif

Color pixelGridColor;

Font unifont;

dint_arr_t font_codepoints;

void ui_loadUnifont() {

    L_D("Loading unifont from %s\n", UNIFONT_PATH);

    size_t size;

    const unsigned char* data = get_resource_data(UNIFONT_PATH, &size);

    if (!data) {

        L_E("Cannot load unifont %s!", UNIFONT_PATH);

        return;
    }

    UnloadFont(unifont);

    unifont = LoadFontFromMemory(
        GetFileExtension(UNIFONT_PATH), data, size, 32, font_codepoints.buffer,
        font_codepoints.size
    );

    free_resource_data((void*)data);
}

void ui_setInitialCodePoints(const char* text) {

    int  codep_count;
    int* codep = LoadCodepoints(text, &codep_count);

    for (int c = 0; c < codep_count; c++) {
        DARRAY_APPEND(font_codepoints, codep[c]);
    }

    UnloadCodepoints(codep);
    ui_loadUnifont();
}

void ui_loadCodepoints(const char* text, bool reload) {

    int  codep_count;
    int  BAD_C = GetGlyphIndex(unifont, '?');
    int* codep = LoadCodepoints(text, &codep_count);

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

void ui_loadCodepointsFromFileList(const doko_control_t* ctrl) {

    DARRAY_FOR_EACH(ctrl->image_files, i) {

        doko_image_t* im = ctrl->image_files.buffer + i;

        ui_loadCodepoints(im->path, false);
    }

    ui_loadUnifont();
}

void ui_init(doko_config_t* config) {

    InitWindow(
        config->window_width, config->window_height, config->window_title
    );
    SetWindowMinSize(config->window_min_width, config->window_min_height);

    SetWindowState(config->window_flags);

    if (config->set_win_position)
        SetWindowPosition(config->window_x, config->window_y);

    SetTargetFPS(WINDOW_FPS);
    SetExitKey(RAYLIB_QUIT_KEY);

    backgroundBuf = ui_loadBackgroundTile(
        BACKGROUND_TILE_W, BACKGROUND_TILE_H,
        (Color)BACKGROUND_TILE_COLOR_A_RGBA, (Color)BACKGROUND_TILE_COLOR_B_RGBA
    );

#if (ENABLE_SHADERS == 1)
    grayscaleShader =
        LoadShaderFromMemory(NULL, INVERT_AND_GRAYSCALE_SHADER_CODE);
    applyInvertShaderValueLocation =
        GetShaderLocation(grayscaleShader, "applyInvert");
    applyGrayscaleShaderValueLocation =
        GetShaderLocation(grayscaleShader, "applyGrayscale");
#endif

    memset(&thumbBufs, 0, sizeof(thumbBufs));

    DARRAY_INIT(font_codepoints, 128);
    ui_setInitialCodePoints(CODEPOINT_INITIAL);

    pixelGridColor = (Color)PIXEL_GRID_COLOR_RGBA;

    imageBuf.id  = 0;
    imageBufPath = NULL;
    hasInit      = 1;
}

void ui_deinit() {

    if (!hasInit) {

        DIE("Cannot deinit if we have no init the ui");
        return;
    }

    DARRAY_FOR_EACH(thumbBufs, i) {
        UnloadTexture(thumbBufs.buffer[i]);
    }
    DARRAY_FREE(thumbBufs);

    UnloadTexture(imageBuf);
    UnloadTexture(backgroundBuf);
    UnloadFont(unifont);

#if (ENABLE_SHADERS == 1)
    UnloadShader(grayscaleShader);
#endif

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

    int sw = GetScreenWidth();
    int sh = ImageViewHeight;

    int fontSize = unifont.baseSize;

    Vector2 textSize;

    do {
        textSize = MeasureTextEx(unifont, text, fontSize, UNIFONT_SPACING);
    } while (textSize.x > sw - INFO_BAR_LEFT_MARGIN && --fontSize);

    DrawRectangle(0, sh, sw, INFO_BAR_HEIGHT, BLACK);

    DrawTextEx(
        unifont, text, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, fontSize,
        UNIFONT_SPACING, WHITE
    );
}

void ui_renderBackground() {

    DrawTextureRec(
        backgroundBuf, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, WHITE
    );
}

void ui_renderImage(doko_image_t* image) {

    if (image->status == IMAGE_STATUS_FAILED) {
        return;
    }

    if (imageBufPath != image->path) {

        if (image->status == IMAGE_STATUS_NOT_LOADED) {

            if (!doko_loadImage(image)) {
                return;
            }

            doko_fitCenterImage(image);
        }

        Texture2D nimageBuf = LoadTextureFromImage(image->rayim);

        if (nimageBuf.id == 0) {
            return;
        }

        if (imageBuf.id > 0) {
            UnloadTexture(imageBuf);
        }

        imageBufPath = image->path;
        imageBuf     = nimageBuf;
    } else if (image->rebuildBuff) {

        image->rebuildBuff = 0;
        UpdateTexture(imageBuf, image->rayim.data);
    }

#if (ENABLE_SHADERS == 1)

    if (image->applyGrayscaleShader || image->applyInvertShader) {

        // these have to be global?? or static for the set value to work
        applyInvertShaderValue    = image->applyInvertShader;
        applyGrayscaleShaderValue = image->applyGrayscaleShader;

        SetShaderValue(
            grayscaleShader, applyInvertShaderValueLocation,
            &applyInvertShaderValue, SHADER_UNIFORM_INT
        );
        SetShaderValue(
            grayscaleShader, applyGrayscaleShaderValueLocation,
            &applyGrayscaleShaderValue, SHADER_UNIFORM_INT
        );

        BeginShaderMode(grayscaleShader);
    }

#endif

    DrawTextureEx(
        imageBuf, image->dstPos, image->rotation, image->scale, WHITE
    );

#if (ENABLE_SHADERS == 1)

    if (image->applyGrayscaleShader | image->applyInvertShader) {

        EndShaderMode();
    }

#endif
}

void ui_renderInfoBar(const doko_image_t* image) {

    const char* prefix = TextFormat(
        "%0.0f x %0.0f  %0.0f%%  ", image->srcRect.width, image->srcRect.height,
        image->scale * 100
    );

    const char* postfix = image->name;

    int sw = GetScreenWidth();
    int sh = ImageViewHeight;

    int fontSize = unifont.baseSize;

    Vector2 pretextSize;
    Vector2 textSize;

    pretextSize = MeasureTextEx(unifont, prefix, fontSize, UNIFONT_SPACING);

    do {
        textSize = MeasureTextEx(unifont, postfix, fontSize, UNIFONT_SPACING);
    } while (textSize.x > (sw - pretextSize.x) && --fontSize);

    DrawRectangle(0, sh, sw, INFO_BAR_HEIGHT, BLACK);

    DrawTextEx(
        unifont, prefix, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, unifont.baseSize,
        UNIFONT_SPACING, WHITE
    );

    DrawTextEx(
        unifont, postfix,
        (Vector2){pretextSize.x, sh + (INFO_BAR_HEIGHT - fontSize) / 2.0},
        fontSize, UNIFONT_SPACING, WHITE
    );
}

void ui_renderPixelGrid(const doko_image_t* image) {

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

#define FZ unifont.baseSize
#define BOTTOM_MARGIN 1

void ui_renderFileList(const doko_control_t* ctrl) {

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight() - FZ;

    int    startY;
    size_t startIndex   = 0;
    size_t scrollOffset = (sh / FZ) / 2;

    if (scrollOffset < ctrl->selected_index &&
        ctrl->image_files.size > scrollOffset * 2) {

        startIndex = ctrl->selected_index - scrollOffset;

        if (startIndex + scrollOffset * 2 > ctrl->image_files.size) {

            startIndex = ctrl->image_files.size - (scrollOffset * 2);
        }
    }

    startY = GetScreenHeight() -
             MIN(ctrl->image_files.size * FZ, (scrollOffset * 2) * FZ) -
             BOTTOM_MARGIN * FZ;

    size_t i = startIndex;

    DARRAY_FOR_I(ctrl->image_files, i, startIndex + scrollOffset * 2) {

        doko_image_t* im = ctrl->image_files.buffer + i;

        int y = startY + FZ * (i - startIndex);

        if (ctrl->selected_image == NULL || i != ctrl->selected_index) {
            DrawRectangle(0, y, sw, FZ, BLACK);
        } else {
            DrawRectangle(0, y, sw, FZ, GREEN);
        }

        DrawTextEx(
            unifont, TextFormat("%02d  %s", i + 1, im->name),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, FZ, UNIFONT_SPACING, WHITE
        );
    }

    DrawRectangle(0, startY + FZ * (i - startIndex), sw, FZ, BLACK);

    DrawTextEx(
        unifont, TextFormat("%d more files...", ctrl->image_files.size - i),
        (Vector2){FILE_LIST_LEFT_MARGIN, startY + FZ * (i - startIndex)}, FZ,
        UNIFONT_SPACING, WHITE
    );
}

void ui_renderKeybinds(const doko_control_t* ctrl) {

    const size_t SCROLL_COUNT = KEYBIND_COUNT + MOUSEBIND_COUNT;
    const int    sw           = GetScreenWidth();
    const int    sh           = GetScreenHeight();
    const char*  PADDING      = "                                 ";

    size_t i;
    int    startY;
    size_t startIndex   = 0;
    size_t scrollOffset = (sh / FZ) / 2;

    if (scrollOffset < ctrl->keybindPageScroll &&
        SCROLL_COUNT > scrollOffset * 2) {

        startIndex = ctrl->keybindPageScroll - scrollOffset;

        if (startIndex + scrollOffset * 2 > SCROLL_COUNT) {

            startIndex = SCROLL_COUNT - (scrollOffset * 2);
        }
    }

    startY =
        GetScreenHeight() - MIN(SCROLL_COUNT * FZ, (scrollOffset * 2) * FZ);

    for (i = startIndex; i < KEYBIND_COUNT; i++) {

        InputMapping* im          = keybinds + i;
        int           y           = startY + FZ * (i - startIndex);
        const char*   SCREEN_TEXT = get_pretty_screen_text(im->screen);
        const char*   KEY_TEXT    = get_key_to_pretty_text(im->key);
        const int     SCR_PAD_LEN = 1 + STRLEN_SCREEN_STR - strlen(SCREEN_TEXT);
        const int     KEY_PAD_LEN = 1 + STRLEN_KEY_STR - strlen(KEY_TEXT);

        if (i != ctrl->keybindPageScroll) {
            DrawRectangle(0, y, sw, FZ, BLACK);
        } else {
            DrawRectangle(0, y, sw, FZ, GREEN);
        }

        DrawTextEx(
            unifont,
            TextFormat(
                "%s%*.*s %s%*.*s %s", SCREEN_TEXT, SCR_PAD_LEN, SCR_PAD_LEN,
                PADDING, KEY_TEXT, KEY_PAD_LEN, KEY_PAD_LEN, PADDING, im->NAME
            ),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, FZ, UNIFONT_SPACING, WHITE
        );
    }

    for (; i - KEYBIND_COUNT < MOUSEBIND_COUNT; i++) {

        InputMapping* im          = mousebinds + i - KEYBIND_COUNT;
        int           y           = startY + FZ * (i - startIndex);
        const char*   SCREEN_TEXT = get_pretty_screen_text(im->screen);
        const char*   KEY_TEXT    = get_mouse_to_pretty_text(im->key);
        const int     SCR_PAD_LEN = 1 + STRLEN_SCREEN_STR - strlen(SCREEN_TEXT);
        const int     KEY_PAD_LEN = 1 + STRLEN_KEY_STR - strlen(KEY_TEXT);

        if (i != ctrl->keybindPageScroll) {
            DrawRectangle(0, y, sw, FZ, BLACK);
        } else {
            DrawRectangle(0, y, sw, FZ, GREEN);
        }

        DrawTextEx(
            unifont,
            TextFormat(
                "%s%*.*s %s%*.*s %s", SCREEN_TEXT, SCR_PAD_LEN, SCR_PAD_LEN,
                PADDING, KEY_TEXT, KEY_PAD_LEN, KEY_PAD_LEN, PADDING, im->NAME
            ),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, FZ, UNIFONT_SPACING, WHITE
        );
    }
}

void ui_renderThumbs(const doko_control_t* ctrl) {

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();

    // ensure we can always get a thumb buffer
    if (ctrl->image_files.length > thumbBufs.length) {

        DARRAY_GROW_SIZE_TO(thumbBufs, ctrl->image_files.length);
    }

    int cols = sw / THUMBNAIL_SIZE;
    int offset = (sw % THUMBNAIL_SIZE) / 2;

    int col = 0;
    int row = 0;

    size_t i = ctrl->thumbPageScroll;

    DARRAY_FOR_EACH_I(ctrl->image_files, i) {

        Image im = ctrl->image_files.buffer[i].thumb;

        if (!IsImageReady(im)) {

            continue;
        }

        Texture2D tex = thumbBufs.buffer[i];

        if (!IsTextureReady(tex)) {

            tex = LoadTextureFromImage(im);

            if (!IsTextureReady(tex)) {
                continue;
            }

            thumbBufs.buffer[i] = tex;
        }

        int x = col * THUMBNAIL_SIZE;
        int y = row * THUMBNAIL_SIZE;

        x -= (x % THUMBNAIL_SIZE);
        x += offset;

        col++;

        if (col >= cols) {
            col = 0;
            row ++;
        }

        DrawTexture(
            tex, 
            x + (THUMBNAIL_SIZE - im.width) / 2,
            y + (THUMBNAIL_SIZE - im.height) / 2, 
            WHITE
        );
        DrawRectangleLines(x, y, THUMBNAIL_SIZE, THUMBNAIL_SIZE, WHITE);
    }
}
