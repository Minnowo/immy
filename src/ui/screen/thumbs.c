
#include <raygui.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "../../darray.h"
#include "../../core/core.h"
#include "../ui.h"

static Texture2D_dynamic_arr_t thumbBufs;

#if ASYNC_IMAGE_LOADING

static int           thumbsLoading = 0;                             // number of thumbs loading
static ImmyImage_t* loadingThumbs[THUMB_ASYNC_LOAD_AMOUNT] = {0 }; // loading thumbs

static inline int getThumbLoadingIndex(ImmyImage_t* im) {

    for (int i = 0; i < THUMB_ASYNC_LOAD_AMOUNT; i++)

        if (loadingThumbs[i] == im)

            return i;

    return -1;
}

static inline void handleThumbLoad(ImmyImage_t* im) {

    switch (im->status) {

    // we can load the thumb
    case IMAGE_STATUS_NOT_LOADED:
        break;

    // wait for it to finish or fail
    case IMAGE_STATUS_LOADING:
        return;

    case IMAGE_STATUS_LOADED:
    case IMAGE_STATUS_FAILED:

        if (thumbsLoading <= 0)
            return;

        int l = getThumbLoadingIndex(im);

        // we did not loading this image
        if (l == -1)
            return;

        // remove it from our list
        thumbsLoading--;
        loadingThumbs[l] = NULL;

        if (im->status == IMAGE_STATUS_FAILED)
            return;

        // create a thumbnail
        iGetOrCreateThumbEx(im, true);

        // if this is false, the user tried to access
        // the image while it was loading for a thumb.
        // In this case, we don't want to free it.
        if (im->isLoadingForThumbOnly) {

            im->isLoadingForThumbOnly = false;
            im->status                = IMAGE_STATUS_NOT_LOADED;

            UnloadImage(im->rayim);
        }

        return;
    }

    if (thumbsLoading >= THUMB_ASYNC_LOAD_AMOUNT)
        return;

    L_I("We are starting to load a new image for it's thumb: %d / %d",
        thumbsLoading, THUMB_ASYNC_LOAD_AMOUNT);

    for (int i = 0; i < THUMB_ASYNC_LOAD_AMOUNT; i++) {

        if(loadingThumbs[i] != NULL)
            continue;

        if (iLoadImageAsync(im)) {

            im->status = IMAGE_STATUS_LOADING;
            im->isLoadingForThumbOnly = true;

            thumbsLoading++;

            loadingThumbs[i] = im;
        }

        break;
    }
}

#endif

void uiThumbPageClearState() {

    DARRAY_FOR_EACH(thumbBufs, i) {
        UnloadTexture(thumbBufs.buffer[i]);
    }

    DARRAY_FREE(thumbBufs);
}


void uiRenderThumbs(const ImmyControl_t* ctrl) {

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();

    // ensure we can always get a thumb buffer
    if (ctrl->image_files.length > thumbBufs.length) {

        DARRAY_GROW_SIZE_TO(thumbBufs, ctrl->image_files.length);
    }

    bool syncLoadedThumb = false;

    int cols   = sw / THUMB_SIZE;
    int rows   = sh / THUMB_SIZE + 1;
    int offset = (sw % THUMB_SIZE) / 2;

    int col = 0;
    int row = 0;

    size_t i = 0;

    // show thumbs around the user's current image
    if(ctrl->selected_index > 2 * cols) {

        i = cols * ((ctrl->selected_index / cols) - 2);
    }

    DARRAY_FOR_EACH_I(ctrl->image_files, i) {

        if(row >= rows)
            continue;

        ImmyImage_t* dim = ctrl->image_files.buffer + i;

#if ASYNC_IMAGE_LOADING
        if (dim->status == IMAGE_STATUS_LOADING) {

            if (!iGetImageAsync(dim))
                continue;

            if (dim->status == IMAGE_STATUS_LOADED)
                uiFitCenterImage(dim);

            handleThumbLoad(dim);
        }
#endif

        if (dim->thumb_status == IMAGE_STATUS_FAILED) {

#if ASYNC_IMAGE_LOADING

            handleThumbLoad(dim);
#else
            if (!syncLoadedThumb &&
                ctrl->frame % (int)SYNC_IMAGE_LOADING_THUMB_INTERVAL == 0 &&
                dim->status == IMAGE_STATUS_NOT_LOADED && immyLoadImage(dim)) {

                syncLoadedThumb = true;
                immyGetOrCreateThumbEx(dim, true);

                dim->status = IMAGE_STATUS_NOT_LOADED;
                UnloadImage(dim->rayim);
            }
#endif

            continue;
        }

        if (dim->thumb_status != IMAGE_STATUS_LOADED) {

            if (!iGetOrCreateThumb(dim)) {

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

        int x = col * THUMB_SIZE;
        int y = row * THUMB_SIZE;

        x -= (x % THUMB_SIZE);
        x += offset;

        col++;

        if (col >= cols) {
            col = 0;
            row ++;
        }

        x += (THUMB_SIZE - dim->thumb.width) / 2.0f;
        y += (THUMB_SIZE - dim->thumb.height) / 2.0f; 

        int pad = 8;
        int m = 2;

        if(i == ctrl->selected_index) {
            DrawRectangle( x, y, dim->thumb.width, dim->thumb.height, uiColorInvert(BAR_BACKGROUND_COLOR_RGBA));
            DrawRectangleLinesEx(
                (Rectangle){
                    x + pad - m, 
                    y + pad - m, 
                    dim->thumb.width  - pad*2 + m*2,
                    dim->thumb.height - pad*2 + m*2
                },
                m, BAR_BACKGROUND_COLOR_RGBA
            );
        }
        else {
            DrawRectangle(x, y, dim->thumb.width, dim->thumb.height, BAR_BACKGROUND_COLOR_RGBA);
            DrawRectangleLinesEx(
                (Rectangle){
                    x + pad - m, 
                    y + pad - m, 
                    dim->thumb.width  - pad*2 + m*2,
                    dim->thumb.height - pad*2 + m*2
                },
                m, uiColorInvert(BAR_BACKGROUND_COLOR_RGBA)
            );
        }


        DrawTexturePro(
            tex,
            (Rectangle){
                0 ,
                0 , 
                dim->thumb.width,
                dim->thumb.height
            },
            (Rectangle){
                pad + x,
                pad + y,
                dim->thumb.width - pad*2, 
                dim->thumb.height - pad*2,
            },
            (Vector2){0, 0}, 0, WHITE
        );
    }
}
