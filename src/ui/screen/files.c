
#include <raygui.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ui.h"
#include "../../config.h"
#include "../../darray.h"
#include "../../core/core.h"

#define BOTTOM_MARGIN 1



void uiRenderFileList(const immy_control_t* ctrl) {

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight() - g_unifontSize;

    int    startY;
    size_t startIndex   = 0;
    size_t scrollOffset = (sh / g_unifontSize) / 2;

    if (scrollOffset < ctrl->selected_index &&
        ctrl->image_files.size > scrollOffset * 2) {

        startIndex = ctrl->selected_index - scrollOffset;

        if (startIndex + scrollOffset * 2 > ctrl->image_files.size) {

            startIndex = ctrl->image_files.size - (scrollOffset * 2);
        }
    }

    startY = GetScreenHeight() -
             MIN(ctrl->image_files.size * g_unifontSize, (scrollOffset * 2) * g_unifontSize) -
             BOTTOM_MARGIN * g_unifontSize;

    size_t i = startIndex;

    DARRAY_FOR_I(ctrl->image_files, i, startIndex + scrollOffset * 2) {

        immy_image_t* im = ctrl->image_files.buffer + i;

        int y = startY + g_unifontSize * (i - startIndex);

        if (ctrl->selected_image == NULL || i != ctrl->selected_index) {
            DrawRectangle(0, y, sw, g_unifontSize, BAR_BACKGROUND_COLOR_RGBA);
        } else {
            DrawRectangle(0, y, sw, g_unifontSize, SELECTED_COLOR_RGBA);
        }

        DrawTextEx(
            g_unifont, TextFormat("%02d  %s", i + 1, im->name),
            (Vector2){FILE_LIST_LEFT_MARGIN, y}, g_unifontSize, UNIFONT_SPACING, TEXT_COLOR_RGBA
        );
    }

    DrawRectangle(0, startY + g_unifontSize * (i - startIndex), sw, g_unifontSize, BAR_BACKGROUND_COLOR_RGBA);

    DrawTextEx(
        g_unifont, TextFormat("%d more files...", ctrl->image_files.size - i),
        (Vector2){FILE_LIST_LEFT_MARGIN, startY + g_unifontSize * (i - startIndex)}, g_unifontSize,
        UNIFONT_SPACING, TEXT_COLOR_RGBA
    );
}
