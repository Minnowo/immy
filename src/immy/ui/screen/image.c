

#include "../../config.h"
#include "../ui.h"
#include <math.h>
#include <string.h>
#include <float.h>

#define ImageViewWidth (GetScreenWidth() - screenPadding.width)
#define ImageViewHeight (GetScreenHeight() - screenPadding.height)

// state for the image screen
static ImmyImage_t* cImage   = 0;   // identify the current image
static Texture2D    imageBuf = {0}; // the buffer to show

// x, y are added to image position
// width, height are subtraced from screen size
static Rectangle screenPadding = {0};

void uiImagePageClearState() {

    UnloadTexture(imageBuf);

    cImage = NULL;

    memset(&imageBuf, 0, sizeof(imageBuf));
}

void uiRenderPixelGrid(const ImmyImage_t* image) {

    float viewportMinX = 0.0f;
    float viewportMinY = 0.0f;
    float viewportMaxX = GetScreenWidth();
    float viewportMaxY = GetScreenHeight();

    float rotation = image->rotation * M_PI / 180.0f;
    float cosTheta = cosf(rotation);
    float sinTheta = sinf(rotation);

    float scale = image->scale;

    float w = image->rayim.width * scale;
    float h = image->rayim.height * scale;

    float localMinX = -w / 2.0f;
    float localMaxX = w / 2.0f;
    float localMinY = -h / 2.0f;
    float localMaxY = h / 2.0f;

    float mx = image->dstPos.x;
    float my = image->dstPos.y;

    for (float x = localMinX; x <= localMaxX; x += scale) {

        float startX = (cosTheta * x - sinTheta * localMinY) + mx;
        float endX   = (cosTheta * x - sinTheta * localMaxY) + mx;
        float startY = (sinTheta * x + cosTheta * localMinY) + my;
        float endY   = (sinTheta * x + cosTheta * localMaxY) + my;

        if ((startX < viewportMinX && endX < viewportMinX) ||
            (startX > viewportMaxX && endX > viewportMaxX) ||
            (startY < viewportMinY && endY < viewportMinY) ||
            (startY > viewportMaxY && endY > viewportMaxY)) {
            continue;
        }

        DrawLine(startX, startY, endX, endY, g_pixelGridColor);
    }

    for (float y = localMinY; y <= localMaxY; y += scale) {

        float startX = (cosTheta * localMinX - sinTheta * y) + mx;
        float endX   = (cosTheta * localMaxX - sinTheta * y) + mx;
        float startY = (sinTheta * localMinX + cosTheta * y) + my;
        float endY   = (sinTheta * localMaxX + cosTheta * y) + my;

        if ((startX < viewportMinX && endX < viewportMinX) ||
            (startX > viewportMaxX && endX > viewportMaxX) ||
            (startY < viewportMinY && endY < viewportMinY) ||
            (startY > viewportMaxY && endY > viewportMaxY)) {
            continue;
        }

        DrawLine(startX, startY, endX, endY, g_pixelGridColor);
    }
}

void uiRenderImage(ImmyControl_t* ctrl, ImmyImage_t* im) {

    if (cImage != im) {

        // ensure the image is not freed if
        // it was being loaded for a thumbnail already
        im->isLoadingForThumbOnly = false;

        switch (im->status) {

#if ASYNC_IMAGE_LOADING

        case IMAGE_STATUS_NOT_LOADED:

            // force rendering so the image loads
            ctrl->renderFrames = RENDER_FRAMES;

            if (!iLoadImageAsync(im)) {

                L_W("Unable to start loading image async");

            } else {

                im->status = IMAGE_STATUS_LOADING;
            }

            return;

        case IMAGE_STATUS_LOADING:

            // force rendering so the image loads
            ctrl->renderFrames = RENDER_FRAMES;

            if (!iGetImageAsync(im)) {

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

            if (!iLoadImage(im)) {
                return;
            }

            uiFitCenterImage(im);
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

        ctrl->renderFrames = RENDER_FRAMES;

        Texture2D nimageBuf = LoadTextureFromImage(im->rayim);

        if (!IsTextureReady(nimageBuf)) {
            return;
        }

        if (IsTextureReady(imageBuf)) {
            UnloadTexture(imageBuf);
        }

        cImage   = im;
        imageBuf = nimageBuf;

        GenTextureMipmaps(&imageBuf);

    } else if (im->rebuildBuff) {

        im->rebuildBuff = 0;
        UpdateTexture(imageBuf, im->rayim.data);
        GenTextureMipmaps(&imageBuf);
    }


#if ENABLE_SHADERS

    if (im->updateShaders) {

        im->updateShaders = false;

        Vector2 effects = {im->applyInvertShader, im->applyGrayscaleShader};

        SetShaderValue(grayscaleShader, grayInvertEffectLocation, &effects, SHADER_UNIFORM_VEC2);
    }

    if (im->applyGrayscaleShader || im->applyInvertShader) {

        BeginShaderMode(grayscaleShader);
    }

#endif

    SetTextureFilter(imageBuf, im->interpolation);

    Rectangle source = {
        0.0f,
        0.0f,
        imageBuf.width * (im->flipX ? -1 : 1),
        imageBuf.height * (im->flipY ? -1 : 1)
    };

    Rectangle dest = {
        im->dstPos.x,
        im->dstPos.y,
        imageBuf.width * im->scale,
        imageBuf.height * im->scale
    };

    Vector2 origin = {
        dest.width / 2.0f,
        dest.height / 2.0f,
    };

    DrawTexturePro(imageBuf, source, dest, origin, im->rotation, WHITE);

#if ENABLE_SHADERS

    if (im->applyGrayscaleShader | im->applyInvertShader) {

        EndShaderMode();
    }

#endif
}

void uiRenderInfoBar(const ImmyImage_t* image) {

    const char* prefix =
        TextFormat("%0.0f x %0.0f  %0.0f%%  ", image->srcRect.width, image->srcRect.height, image->scale * 100);

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
        g_unifont, prefix, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, g_unifont.baseSize, UNIFONT_SPACING, TEXT_COLOR_RGBA
    );

    DrawTextEx(
        g_unifont,
        postfix,
        (Vector2){pretextSize.x, sh + (INFO_BAR_HEIGHT - fontSize) / 2.0},
        fontSize,
        UNIFONT_SPACING,
        TEXT_COLOR_RGBA
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

    DrawTextEx(g_unifont, text, (Vector2){INFO_BAR_LEFT_MARGIN, sh}, fontSize, UNIFONT_SPACING, TEXT_COLOR_RGBA);
}

void uiFitCenterImage(ImmyImage_t* image) {

    int sw = ImageViewWidth - screenPadding.x;
    int sh = ImageViewHeight - screenPadding.y;
    float iw = image->srcRect.width;
    float ih = image->srcRect.height;

    image->scale = fminf(sw / iw, sh / ih);

    image->dstPos.x = screenPadding.x + (sw / 2.0f);
    image->dstPos.y = screenPadding.y + (sh / 2.0f);
}

void uiCenterImage(ImmyImage_t* image) {

    int sw = ImageViewWidth - screenPadding.x;
    int sh = ImageViewHeight - screenPadding.y;
    float iw = image->srcRect.width;
    float ih = image->srcRect.height;

    image->dstPos.x = screenPadding.x + (sw / 2.0f);
    image->dstPos.y = screenPadding.y + (sh / 2.0f);
}

void uiEnsureImageNotLost(ImmyImage_t* image) {

    float rotation = image->rotation * M_PI / 180.0f;
    float cosTheta = cosf(rotation);
    float sinTheta = sinf(rotation);

    float corners[] = {-1, -1, 1, -1, 1, 1, -1, 1};

    float minX = FLT_MAX;
    float maxX = -FLT_MAX;
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;

    float ihw = (image->srcRect.width * image->scale) / 2.0f;
    float ihh = (image->srcRect.height * image->scale) / 2.0f;

    for (int i = 0; i < 4; ++i) {
        float localX = ihw * corners[i * 2];
        float localY = ihh * corners[i * 2 + 1];

        float rotatedX = cosTheta * localX - sinTheta * localY;
        float rotatedY = sinTheta * localX + cosTheta * localY;

        rotatedX += image->dstPos.x;
        rotatedY += image->dstPos.y;

        if (rotatedX < minX) minX = rotatedX;
        if (rotatedX > maxX) maxX = rotatedX;
        if (rotatedY < minY) minY = rotatedY;
        if (rotatedY > maxY) maxY = rotatedY;
    }

    float viewportMinX = screenPadding.x + IMAGE_INVERSE_MARGIN_X;
    float viewportMinY = screenPadding.y + IMAGE_INVERSE_MARGIN_Y;
    float viewportMaxX = ImageViewWidth - IMAGE_INVERSE_MARGIN_X;
    float viewportMaxY = ImageViewHeight - IMAGE_INVERSE_MARGIN_Y;

    if (maxX < viewportMinX) {
        image->dstPos.x += (viewportMinX - maxX);
    } else if (minX > viewportMaxX) {
        image->dstPos.x -= (minX - viewportMaxX);
    }

    if (maxY < viewportMinY) {
        image->dstPos.y += (viewportMinY - maxY);
    } else if (minY > viewportMaxY) {
        image->dstPos.y -= (minY - viewportMaxY);
    }
}

void uiMoveScrFracImage(ImmyImage_t* im, float xFrac, float yFrac) {

    im->dstPos.x += (ImageViewWidth - screenPadding.x) * xFrac;
    im->dstPos.y += (ImageViewHeight - screenPadding.y) * yFrac;
    uiEnsureImageNotLost(im);
}

void uiZoomImageCenter(ImmyImage_t* im, float afterZoom) {

    float beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    im->dstPos.x -= (int)((im->srcRect.width * afterZoom) - (im->srcRect.width * beforeZoom)) >> 1;
    im->dstPos.y -= (int)((im->srcRect.height * afterZoom) - (im->srcRect.height * beforeZoom)) >> 1;

    im->scale = afterZoom;

    uiEnsureImageNotLost(im);
}


void uiZoomImageOnPoint(ImmyImage_t* im, float afterZoom, int x, int y) {

    float beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    float rotation = im->rotation * M_PI / 180;

    float cosTheta = cosf(rotation);
    float sinTheta = sinf(rotation);

    float mouseOffsetX = x - im->dstPos.x;
    float mouseOffsetY = y - im->dstPos.y;

    float scaleRatio = (im->srcRect.width * beforeZoom) / (im->srcRect.width * afterZoom);

    float localMouseX =  cosTheta * mouseOffsetX + sinTheta * mouseOffsetY;
    float localMouseY = -sinTheta * mouseOffsetX + cosTheta * mouseOffsetY;

    float adjustedLocalMouseX = localMouseX / scaleRatio;
    float adjustedLocalMouseY = localMouseY / scaleRatio;

    float finalMouseOffsetX = cosTheta * adjustedLocalMouseX - sinTheta * adjustedLocalMouseY;
    float finalMouseOffsetY = sinTheta * adjustedLocalMouseX + cosTheta * adjustedLocalMouseY;

    im->dstPos.x += (mouseOffsetX - finalMouseOffsetX);
    im->dstPos.y += (mouseOffsetY - finalMouseOffsetY);

    im->scale = afterZoom;

    uiEnsureImageNotLost(im);
}

void uiZoomImageCenterFromClosest(ImmyImage_t* im, bool zoomIn) {

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

void uiZoomImageOnPointFromClosest(ImmyImage_t* im, bool zoomIn, int x, int y) {

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

