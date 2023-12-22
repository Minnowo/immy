

#include "keybind.h"
#include "doko.h"
#include <raylib.h>
#include <stdio.h>

#define IM(controller)                                                         \
    ((controller)->image_files.buffer + (controller)->selected_file)

void keybind_zoomInCenterImage(doko_control_t *ctrl) {
    doko_zoomImageOnPointFromClosest(IM(ctrl), true, GetScreenWidth() / 2,
                                     GetScreenHeight() / 2);
}

void keybind_zoomOutCenterImage(doko_control_t *ctrl) {
    doko_zoomImageOnPointFromClosest(IM(ctrl), false, GetScreenWidth() / 2,
                                     GetScreenHeight() / 2);
}

void keybind_nextImage(doko_control_t *ctrl) {
    ctrl->selected_file = (ctrl->selected_file + 1) % ctrl->image_files.size;
}

void keybind_PrevImage(doko_control_t *ctrl) {
    ctrl->selected_file = (ctrl->selected_file + ctrl->image_files.size - 1) %
                          ctrl->image_files.size;
}

void keybind_printDebugInfo(doko_control_t *ctrl) {

    doko_image_t *im;

    DARRAY_FOR_EACH_I(ctrl->image_files, i) {

        im = ctrl->image_files.buffer + i;

        printf("%zu: %s\n", i, im->path);
    }

    im = IM(ctrl);

    printf("Image[%zu]:\n", ctrl->selected_file);
    printf("   Size     %0.1f %0.1f\n", im->srcRect.width, im->srcRect.height);
    printf("   Visible  %0.1f %0.1f\n", im->srcRect.width * im->scale,
           im->srcRect.height * im->scale);
    printf("   Position %0.1f %0.1f\n", im->dstPos.x, im->dstPos.y);
    printf("   Scale %f\n", im->scale);
}

void keybind_moveImageUp(doko_control_t *ctrl) {
    doko_moveScrFracImage(IM(ctrl), 0, -1 / 5.0);
}

void keybind_moveImageDown(doko_control_t *ctrl) {
    doko_moveScrFracImage(IM(ctrl), 0, 1 / 5.0);
}

void keybind_moveImageLeft(doko_control_t *ctrl) {
    doko_moveScrFracImage(IM(ctrl), -1 / 5.0, 0);
}

void keybind_moveImageRight(doko_control_t *ctrl) {
    doko_moveScrFracImage(IM(ctrl), 1 / 5.0, 0);
}

void keybind_centerImage(doko_control_t *ctrl) {
    doko_centerImage(IM(ctrl));
}

void keybind_fitCenterImage(doko_control_t *ctrl) {
    doko_fitCenterImage(IM(ctrl));
}

void keybind_flipVertical(doko_control_t *ctrl) {
    doko_image_t *im = IM(ctrl);
    ImageFlipVertical(&im->rayim);
    im->rebuildBuff = 1;
}

void keybind_flipHorizontal(doko_control_t *ctrl) {
    doko_image_t *im = IM(ctrl);
    ImageFlipHorizontal(&im->rayim);
    im->rebuildBuff = 1;
}

void keybind_colorInvert(doko_control_t *ctrl) {
    doko_image_t *im = IM(ctrl);
    ImageColorInvert(&im->rayim);
    im->rebuildBuff = 1;
}

void keybind_increaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() + 1);
}

void keybind_decreaseFPS(doko_control_t *ctrl) {
    SetTargetFPS(GetFPS() - 1);
}
