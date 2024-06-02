

#include "../../config.h"
#include "../ui.h"
#include <string.h>

// state for the image screen
static doko_image_t* image    = 0;   // identify the current image
static Texture2D     imageBuf = {0}; // the buffer to show

void uiImagePageClearState() {

    UnloadTexture(imageBuf);

    image = NULL;

    memset(&imageBuf, 0, sizeof(imageBuf));
}

void uiRenderPixelGrid(const doko_image_t* image) {

    float x = image->dstPos.y;
    float y = image->dstPos.x;
    float w = ImageViewWidth;
    float h = ImageViewHeight;

    for (float i = x, j = y; i <= h || j <= w;) {
        DrawLine(0, i, w, i, g_pixelGridColor);
        DrawLine(j, 0, j, h, g_pixelGridColor);
        i += image->scale;
        j += image->scale;
    }

    for (float i = x, j = y; i > 0 || j > 0;) {
        DrawLine(0, i, w, i, g_pixelGridColor);
        DrawLine(j, 0, j, h, g_pixelGridColor);
        i -= image->scale;
        j -= image->scale;
    }
}

void uiRenderImage(doko_image_t* im) {

    if (image != im) {

        switch(im->status){

#if(ASYNC_IMAGE_LOADING == 1)

        case IMAGE_STATUS_NOT_LOADED:

            if (!doko_async_load_image(im)) {

                L_W("Unable to start loading image async");

            } else {

                im->status = IMAGE_STATUS_LOADING;
            }

            return;

        case IMAGE_STATUS_LOADING:

            if(!doko_async_get_image(im)) {

                uiDrawText("image is loading");

                return;
            }

            if (im->status == IMAGE_STATUS_LOADED)
                doko_fitCenterImage(im);

            // if the above function returns true
            // the image is done loading and has a new status
            // if we return here the new status will be handled next frame
            return;

#else
        case IMAGE_STATUS_NOT_LOADED:

            if (!doko_loadImage(image)) {
                return;
            }

            doko_fitCenterImage(image);
            break;

        case IMAGE_STATUS_LOADING:

            L_E("%s: This should be unreachable when ASYNC_IMAGE_LOADING is not true", __func__);

            uiDrawText("This should be unreachable when ASYNC_IMAGE_LOADING is not 1");

            return;
#endif

        case IMAGE_STATUS_LOADED:
            break;

        case IMAGE_STATUS_FAILED:
            uiDrawText("The image could not be loaded!");
            return;
        }


        Texture2D nimageBuf = LoadTextureFromImage(im->rayim);

        if (!IsTextureReady(nimageBuf)) {
            return;
        }

        if (IsTextureReady(imageBuf)) {
            UnloadTexture(imageBuf);
        }

        image    = im;
        imageBuf = nimageBuf;

        GenTextureMipmaps(&imageBuf);

    } else if (im->rebuildBuff) {

        im->rebuildBuff = 0;
        UpdateTexture(imageBuf, im->rayim.data);
        GenTextureMipmaps(&imageBuf);
    }

#if (ENABLE_SHADERS == 1)

    if (im->applyGrayscaleShader || im->applyInvertShader) {

        // these have to be global?? or static for the set value to work
        applyInvertShaderValue    = image->applyInvertShader;
        applyGrayscaleShaderValue = image->applyGrayscaleShader;

        SetShaderValue(
            grayscaleShader, 
            invertShaderValueLocation, 
            &applyInvertShaderValue,
            SHADER_UNIFORM_INT
        );
        SetShaderValue(
            grayscaleShader, 
            grayscaleShaderValueLocation,
            &applyGrayscaleShaderValue, 
            SHADER_UNIFORM_INT
        );

        BeginShaderMode(grayscaleShader);
    }

#endif

    SetTextureFilter(imageBuf, im->interpolation);

    DrawTextureEx(
        imageBuf, 
        im->dstPos, 
        im->rotation, 
        im->scale, 
        WHITE
    );

#if (ENABLE_SHADERS == 1)

    if (im->applyGrayscaleShader | im->applyInvertShader) {

        EndShaderMode();
    }

#endif
}


void uiRenderInfoBar(const doko_image_t* image) {

    const char* prefix = TextFormat("%0.0f x %0.0f  %0.0f%%  ", 
                                    image->srcRect.width, 
                                    image->srcRect.height,
                                    image->scale * 100);

    const char* postfix = image->name;

    int sw = GetScreenWidth();
    int sh = ImageViewHeight;

    int fontSize = g_unifontSize;

    Vector2 pretextSize;
    Vector2 textSize;

    pretextSize = MeasureTextEx(g_unifont, prefix, fontSize, UNIFONT_SPACING);

    do {
        textSize = MeasureTextEx(g_unifont, postfix, fontSize, UNIFONT_SPACING);
    } while (textSize.x > (sw - pretextSize.x) && --fontSize);

    DrawRectangle(0, sh, sw, INFO_BAR_HEIGHT, BAR_BACKGROUND_COLOR_RGBA);

    DrawTextEx(
        g_unifont, prefix, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, g_unifont.baseSize,
        UNIFONT_SPACING, TEXT_COLOR_RGBA
    );

    DrawTextEx(
        g_unifont, postfix,
        (Vector2){pretextSize.x, sh + (INFO_BAR_HEIGHT - fontSize) / 2.0},
        fontSize, UNIFONT_SPACING, TEXT_COLOR_RGBA
    );
}


void uiRenderTextOnInfoBar(const char* text) {

    int sw = GetScreenWidth();
    int sh = ImageViewHeight;

    int fontSize = g_unifont.baseSize;

    Vector2 textSize;

    do {
        textSize = MeasureTextEx(g_unifont, text, fontSize, UNIFONT_SPACING);
    } while (textSize.x > sw - INFO_BAR_LEFT_MARGIN && --fontSize);

    DrawRectangle(0, sh, sw, INFO_BAR_HEIGHT, BAR_BACKGROUND_COLOR_RGBA);

    DrawTextEx(
        g_unifont, text, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, fontSize,
        UNIFONT_SPACING, TEXT_COLOR_RGBA
    );
}

