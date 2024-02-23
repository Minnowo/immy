
#include "input.h"
#include "doko.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define _NO_IMAGE_WARN(c)                                                      \
    if ((c)->selected_image == NULL) {                                         \
        L_W("There is no image!");                                             \
        return;                                                                \
    }

#define I_X(i) (i)->selected_image->dstPos.x
#define I_Y(i) (i)->selected_image->dstPos.y
#define I_WIDTH(i) (i)->selected_image->srcRect.width
#define I_HEIGHT(i) (i)->selected_image->srcRect.height
#define I_SCALE(i) (i)->selected_image->scale

void keybind_zoomInCenterImage(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_zoomImageOnPointFromClosest(ctrl->selected_image, true, GetScreenWidth() / 2,
                                     GetScreenHeight() / 2);
}

void keybind_zoomOutCenterImage(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_zoomImageOnPointFromClosest(ctrl->selected_image, false, GetScreenWidth() / 2,
                                     GetScreenHeight() / 2);
}

void keybind_zoomInMousePosition(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_zoomImageOnPointFromClosest(ctrl->selected_image, true, GetMouseX(), GetMouseY());
}
void keybind_zoomOutMousePosition(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_zoomImageOnPointFromClosest(ctrl->selected_image, false, GetMouseX(), GetMouseY());
}

void keybind_nextImageN(doko_control_t *ctrl, int by) {

    if(ctrl->image_files.size == 0) {
        ctrl->selected_image = NULL;
    }

    _NO_IMAGE_WARN(ctrl);

    set_image(ctrl, (ctrl->selected_index + by) % ctrl->image_files.size);
}


void keybind_prevImageN(doko_control_t *ctrl, int by) {

    if(ctrl->image_files.size == 0) {
        ctrl->selected_image = NULL;
    }

    _NO_IMAGE_WARN(ctrl);

    set_image(ctrl, (ctrl->selected_index + ctrl->image_files.size - by) % ctrl->image_files.size);
}

void keybind_nextImage(doko_control_t *ctrl) {

    keybind_nextImageN(ctrl, 1);
}

void keybind_nextImageBy10(doko_control_t *ctrl) {

    keybind_nextImageN(ctrl, 10);
}

void keybind_prevImage(doko_control_t *ctrl) {

    keybind_prevImageN(ctrl, 1);
}

void keybind_prevImageBy10(doko_control_t *ctrl){

    keybind_prevImageN(ctrl, 10);
}

void keybind_jumpImageN(doko_control_t* ctrl, int to) {

    if(ctrl->image_files.size == 0) {
        ctrl->selected_image = NULL;
    }

    _NO_IMAGE_WARN(ctrl);

    set_image(ctrl, to);
}

void keybind_jumpImageStart(doko_control_t* ctrl) {

    keybind_jumpImageN(ctrl, 0);

}

void keybind_jumpImageEnd(doko_control_t* ctrl) {

    keybind_jumpImageN(ctrl, ctrl->image_files.size - 1);
}

void keybind_printDebugInfo(doko_control_t *ctrl) {

    doko_image_t *im;

    DARRAY_FOR_EACH_I(ctrl->image_files, i) {

        im = ctrl->image_files.buffer + i;

        printf("%zu: %s\n", i, im->path);
    }

    im = ctrl->selected_image;

    _NO_IMAGE_WARN(ctrl);

    printf("Image[%zu]:\n", ctrl->selected_index);
    printf("   Size     %0.1f %0.1f\n", im->srcRect.width, im->srcRect.height);
    printf("   Visible  %0.1f %0.1f\n", im->srcRect.width * im->scale,
           im->srcRect.height * im->scale);
    printf("   Position %0.1f %0.1f\n", im->dstPos.x, im->dstPos.y);
    printf("   Scale %f\n", im->scale);
}

void keybind_moveImageUp(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_moveScrFracImage(ctrl->selected_image, 0, -1 / 5.0);
}

void keybind_moveImageDown(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_moveScrFracImage(ctrl->selected_image, 0, 1 / 5.0);
}

void keybind_moveImageLeft(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_moveScrFracImage(ctrl->selected_image, -1 / 5.0, 0);
}

void keybind_moveImageRight(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_moveScrFracImage(ctrl->selected_image, 1 / 5.0, 0);
}

void keybind_moveImageByMouseDelta(doko_control_t *ctrl) {

    _NO_IMAGE_WARN(ctrl);

    doko_image_t *im = ctrl->selected_image;

    // // ctrl->lastMouseClick is set after taking input now
    // // so we don't need to set it here, and this can work with any input

    // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //     ctrl->lastMouseClick = mp;
    // }

    // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

        im->dstPos.x -= ctrl->lastMouseClick.x - GetMouseX();
        im->dstPos.y -= ctrl->lastMouseClick.y - GetMouseY();

        // ctrl->lastMouseClick = mp;
    // }
}

void keybind_centerImage(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_centerImage(ctrl->selected_image);
}

void keybind_fitCenterImage(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    doko_fitCenterImage(ctrl->selected_image);
}

void keybind_flipVertical(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);

    float hh = GetScreenHeight() / 2.0;

    I_Y(ctrl) = hh + (hh - I_Y(ctrl)) - (I_SCALE(ctrl) * I_HEIGHT(ctrl));

    ImageFlipVertical(&ctrl->selected_image->rayim);
    ctrl->selected_image->rebuildBuff = 1;
}

void keybind_flipHorizontal(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);

    float hw = GetScreenWidth() / 2.0;

    I_X(ctrl) = hw + (hw - I_X(ctrl)) - (I_SCALE(ctrl) * I_WIDTH(ctrl));

    ImageFlipHorizontal(&ctrl->selected_image->rayim);
    ctrl->selected_image->rebuildBuff = 1;
}

void keybind_colorInvert(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);

    ctrl->selected_image->rebuildBuff = true;

    int bytes = 4;

    switch (ctrl->selected_image->rayim.format) {

    default:
        ImageColorInvert(&ctrl->selected_image->rayim);
        break;

    case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
    case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
        bytes = 3;
        // fallthrough

    case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
    case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: {

        unsigned char* pixels = ctrl->selected_image->rayim.data;

        size_t size = bytes * ctrl->selected_image->rayim.width * ctrl->selected_image->rayim.height;

        for (size_t i = 0; i < size; i += (bytes == 4)) {

            pixels[i] = 255 - pixels[i];
            ++i;
            pixels[i] = 255 - pixels[i];
            ++i;
            pixels[i] = 255 - pixels[i];
            ++i;
        }
        break;
    }
    }
}

void keybind_colorInvertShader(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    ctrl->selected_image->applyInvertShader = !ctrl->selected_image->applyInvertShader;
}

void keybind_colorGrayscaleShader(doko_control_t* ctrl) {
    _NO_IMAGE_WARN(ctrl);
    ctrl->selected_image->applyGrayscaleShader = !ctrl->selected_image->applyGrayscaleShader;
}

void keybind_increaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() + 1);
}

void keybind_decreaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() - 1);
}

void keybind_cycleScreen(doko_control_t *ctrl) {

    ctrl->screen++;

    if(ctrl->screen > DOKO_SCREEN__END) {

        ctrl->screen = DOKO_SCREEN__START;
    }
}

void keybind_cycleScreenReverse(doko_control_t *ctrl) {

    ctrl->screen--;

    if(ctrl->screen < DOKO_SCREEN__START) {

        ctrl->screen = DOKO_SCREEN__END;
    }
}
