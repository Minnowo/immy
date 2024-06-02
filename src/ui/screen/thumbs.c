
#include <raygui.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "../../darray.h"
#include "../../doko.h"
#include "../ui.h"

static Texture2D_dynamic_arr_t thumbBufs;

void uiThumbPageClearState() {

    DARRAY_FOR_EACH(thumbBufs, i) {
        UnloadTexture(thumbBufs.buffer[i]);
    }

    DARRAY_FREE(thumbBufs);
}


void uiRenderThumbs(const doko_control_t* ctrl) {

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

        doko_image_t* dim = ctrl->image_files.buffer + i;

#ifdef ASYNC_IMAGE_LOADING
        if (dim->status == IMAGE_STATUS_LOADING) {

            if (!doko_async_get_image(dim))
                continue;

            if (dim->status == IMAGE_STATUS_LOADED)
                doko_fitCenterImage(dim);
        }
#endif

        if (dim->status != IMAGE_STATUS_LOADED ||
            dim->thumb_status == IMAGE_STATUS_FAILED) {
            continue;
        }

        if (dim->thumb_status != IMAGE_STATUS_LOADED) {

            if (!doko_create_thumbnail( &dim->rayim, &dim->thumb, THUMBNAIL_SIZE, THUMBNAIL_SIZE)) {

                L_E("Error creating thumbnail!");

                memset(&dim->thumb, 0, sizeof(dim->thumb));

                dim->thumb_status = IMAGE_STATUS_FAILED;

                continue;
            }

            dim->thumb_status = IMAGE_STATUS_LOADED;
        }

        Texture2D tex = thumbBufs.buffer[i];

        if (!IsTextureReady(tex)) {

            tex = LoadTextureFromImage(dim->thumb);

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
            x + (THUMBNAIL_SIZE - dim->thumb.width) / 2,
            y + (THUMBNAIL_SIZE - dim->thumb.height) / 2, 
            WHITE
        );

        if(i == ctrl->selected_index) {
            DrawRectangleLines(x, y, THUMBNAIL_SIZE, THUMBNAIL_SIZE, RED);
        }
        else {
            DrawRectangleLines(x, y, THUMBNAIL_SIZE, THUMBNAIL_SIZE, TEXT_COLOR_RGBA);
        }
    }
}
