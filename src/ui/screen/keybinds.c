

#include <raygui.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "../../core/core.h"
#include "../ui.h"

void uiRenderKeybinds(const ImmyControl_t* ctrl) {

    const size_t SCROLL_COUNT = KEYBIND_COUNT + MOUSEBIND_COUNT;
    const int    sw           = GetScreenWidth();
    const int    sh           = GetScreenHeight();
    const char*  PADDING      = "                                 ";

    size_t i;
    int    startY;
    size_t startIndex   = 0;
    size_t scrollOffset = (sh / g_unifontSize) / 2;

    if (scrollOffset < ctrl->keybindPageScroll && SCROLL_COUNT > scrollOffset * 2) {

        startIndex = ctrl->keybindPageScroll - scrollOffset;

        if (startIndex + scrollOffset * 2 > SCROLL_COUNT) {

            startIndex = SCROLL_COUNT - (scrollOffset * 2);
        }
    }

    startY = GetScreenHeight() - MIN(SCROLL_COUNT * g_unifontSize, (scrollOffset * 2) * g_unifontSize);

    for (i = startIndex; i < KEYBIND_COUNT; i++) {

        InputMapping* im = keybinds + i;
        int           y  = startY + g_unifontSize * (i - startIndex);

        const char*   SCREEN_TEXT = iScreenToStr(im->screen);
        const char*   KEY_TEXT    = iKeyToStr(im->key);
        const int     SCR_PAD_LEN = 1 + STRLEN_SCREEN_STR - strlen(SCREEN_TEXT);
        const int     KEY_PAD_LEN = 1 + STRLEN_KEY_STR - strlen(KEY_TEXT);

        if (i != ctrl->keybindPageScroll) {
            DrawRectangle(0, y, sw, g_unifontSize, BAR_BACKGROUND_COLOR_RGBA);
        } else {
            DrawRectangle(0, y, sw, g_unifontSize, SELECTED_COLOR_RGBA);
        }

        DrawTextEx(
            g_unifont,
            TextFormat(
                "%s%*.*s %s%*.*s %s", SCREEN_TEXT, SCR_PAD_LEN, SCR_PAD_LEN, PADDING, KEY_TEXT, KEY_PAD_LEN,
                KEY_PAD_LEN, PADDING, im->NAME
            ),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, g_unifontSize, UNIFONT_SPACING, TEXT_COLOR_RGBA
        );
    }

    for (; i - KEYBIND_COUNT < MOUSEBIND_COUNT; i++) {

        InputMapping* im = mousebinds + i - KEYBIND_COUNT;
        int           y  = startY + g_unifontSize * (i - startIndex);

        const char*   SCREEN_TEXT = iScreenToStr(im->screen);
        const char*   KEY_TEXT    = iKeyToStr(im->key);
        const int     SCR_PAD_LEN = 1 + STRLEN_SCREEN_STR - strlen(SCREEN_TEXT);
        const int     KEY_PAD_LEN = 1 + STRLEN_KEY_STR - strlen(KEY_TEXT);

        if (i != ctrl->keybindPageScroll) {
            DrawRectangle(0, y, sw, g_unifontSize, BAR_BACKGROUND_COLOR_RGBA);
        } else {
            DrawRectangle(0, y, sw, g_unifontSize, SELECTED_COLOR_RGBA);
        }

        DrawTextEx(
            g_unifont,
            TextFormat(
                "%s%*.*s %s%*.*s %s", SCREEN_TEXT, SCR_PAD_LEN, SCR_PAD_LEN, PADDING, KEY_TEXT, KEY_PAD_LEN,
                KEY_PAD_LEN, PADDING, im->NAME
            ),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, g_unifontSize, UNIFONT_SPACING, TEXT_COLOR_RGBA
        );
    }
}
