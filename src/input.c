

#include "input.h"
#include "doko.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define _NO_IMAGE_WARN(c)                                                      \
    if ((c)->selected_image == NULL) {                                         \
        L_D("There is no image!");                                             \
        return;                                                                \
    }

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

void keybind_nextImage(doko_control_t *ctrl) {
    if(ctrl->image_files.size == 0) {
        ctrl->selected_image = NULL;
    }
    _NO_IMAGE_WARN(ctrl);

    set_image(ctrl, (ctrl->selected_index + 1) % ctrl->image_files.size);
}

void keybind_PrevImage(doko_control_t *ctrl) {
    if(ctrl->image_files.size == 0) {
        ctrl->selected_image = NULL;
    }
    _NO_IMAGE_WARN(ctrl);
    set_image(ctrl, (ctrl->selected_index + ctrl->image_files.size - 1) %
                        ctrl->image_files.size);
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
    ImageFlipVertical(&ctrl->selected_image->rayim);
    ctrl->selected_image->rebuildBuff = 1;
}

void keybind_flipHorizontal(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    ImageFlipHorizontal(&ctrl->selected_image->rayim);
    ctrl->selected_image->rebuildBuff = 1;
}

void keybind_colorInvert(doko_control_t *ctrl) {
    _NO_IMAGE_WARN(ctrl);
    ImageColorInvert(&ctrl->selected_image->rayim);
    ctrl->selected_image->rebuildBuff = 1;
}

void keybind_increaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() + 1);
}

void keybind_decreaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() - 1);
}
