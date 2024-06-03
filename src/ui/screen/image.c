

#include "../../config.h"
#include "../ui.h"
#include <math.h>
#include <string.h>

#define ImageViewWidth (GetScreenWidth() - screenPadding.width)
#define ImageViewHeight (GetScreenHeight() - screenPadding.height)

// state for the image screen
static doko_image_t* image    = 0;   // identify the current image
static Texture2D     imageBuf = {0}; // the buffer to show

// x, y are added to image position
// width, height are subtraced from screen size
static Rectangle screenPadding = {0};

void uiImagePageClearState() {

    UnloadTexture(imageBuf);

    image = NULL;

    memset(&imageBuf, 0, sizeof(imageBuf));
}

void uiRenderPixelGrid(const doko_image_t* image) {

    float x = image->dstPos.y;
    float y = image->dstPos.x;
    float w = GetScreenWidth();
    float h = GetScreenHeight();

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
                uiFitCenterImage(im);

            // if the above function returns true
            // the image is done loading and has a new status
            // if we return here the new status will be handled next frame
            return;

#else
        case IMAGE_STATUS_NOT_LOADED:

            if (!doko_loadImage(image)) {
                return;
            }

            uiFitCenterImage(image);
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

void uiFitCenterImage(doko_image_t* image) {

    int sw = ImageViewWidth - screenPadding.x;
    int sh = ImageViewHeight - screenPadding.y;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->scale = fmin((double)sw / iw, (double)sh / ih);

    image->dstPos.x = screenPadding.x + (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = screenPadding.y + (sh / 2.0) - (ih * image->scale) / 2.0;
}

void uiCenterImage(doko_image_t* image) {

    int sw = ImageViewWidth - screenPadding.x;
    int sh = ImageViewHeight - screenPadding.y;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->dstPos.x = screenPadding.x + (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = screenPadding.y + (sh / 2.0) - (ih * image->scale) / 2.0;
}

void uiEnsureImageNotLost(doko_image_t* image) {

    int   sw = ImageViewWidth - IMAGE_INVERSE_MARGIN_X;
    int   sh = ImageViewHeight - IMAGE_INVERSE_MARGIN_Y;
    float iw = (screenPadding.x + IMAGE_INVERSE_MARGIN_X) - (image->srcRect.width * image->scale);
    float ih = (screenPadding.y + IMAGE_INVERSE_MARGIN_Y) -(image->srcRect.height * image->scale);

    if (image->dstPos.x > sw) {

        image->dstPos.x = sw;

    } else if (image->dstPos.x < iw) {

        image->dstPos.x = iw;
    }

    if (image->dstPos.y > sh) {

        image->dstPos.y = sh;

    } else if (image->dstPos.y < ih) {

        image->dstPos.y = ih;
    }
}

void uiMoveScrFracImage(doko_image_t* im, double xFrac, double yFrac) {

    im->dstPos.x += (ImageViewWidth - screenPadding.x) * xFrac;
    im->dstPos.y += (ImageViewHeight - screenPadding.y) * yFrac;
    uiEnsureImageNotLost(im);
}

void uiZoomImageCenter(doko_image_t* im, double afterZoom) {

    double beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    //clang-format off
    im->dstPos.x -= (int)((im->srcRect.width * afterZoom) -
                          (im->srcRect.width * beforeZoom)) >> 1;

    im->dstPos.y -= (int)((im->srcRect.height * afterZoom) -
                          (im->srcRect.height * beforeZoom)) >> 1;
    //clang-format on

    im->scale = afterZoom;

    uiEnsureImageNotLost(im);
}

void uiZoomImageOnPoint(doko_image_t* im, double afterZoom, int x, int y) {

    double beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    double scaleRatio =
        (im->srcRect.width * beforeZoom) / (im->srcRect.width * afterZoom);

    double mouseOffsetX = x - im->dstPos.x;
    double mouseOffsetY = y - im->dstPos.y;

    im->dstPos.x -= (int)((mouseOffsetX / scaleRatio) - mouseOffsetX);
    im->dstPos.y -= (int)((mouseOffsetY / scaleRatio) - mouseOffsetY);
    im->scale = afterZoom;

    uiEnsureImageNotLost(im);
}

void uiZoomImageCenterFromClosest(doko_image_t* im, bool zoomIn) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {

        while (index + 1 < ZOOM_LEVELS_SIZE && ZOOM_LEVELS[index] <= im->scale) {
            index++;
        }

    } else {

        while (index != 0 && ZOOM_LEVELS[index] >= im->scale) {
            index--;
        }
    }

    uiZoomImageCenter(im, ZOOM_LEVELS[index]);
}

void uiZoomImageOnPointFromClosest(
    doko_image_t* im, bool zoomIn, int x, int y
) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {

        while (index + 1 < ZOOM_LEVELS_SIZE && ZOOM_LEVELS[index] <= im->scale) {
            index++;
        }

    } else {

        while (index != 0 && ZOOM_LEVELS[index] >= im->scale) {
            index--;
        }
    }

    uiZoomImageOnPoint(im, ZOOM_LEVELS[index], x, y);
}


void uiSetScreenPadding(Rectangle vp) {
    screenPadding = vp;
}

Rectangle uiGetScreenPadding() {
    return screenPadding;
}

void uiSetScreenPaddingRight(float width) {
    screenPadding.width = width;
}

void uiSetScreenPaddingBottom(float height) {
    screenPadding.height = height;
}

void uiSetScreenPaddingLeft(float x) {
    screenPadding.x = x;
}

void uiSetScreenPaddingTop(float y) {
    screenPadding.y = y;
}

