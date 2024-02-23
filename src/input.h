

#ifndef DOKO_KEYBINDS_H
#define DOKO_KEYBINDS_H

#include "doko.h"
#include <raylib.h>

#define SHIFT_MASK   0x80000000  // 1000 0000 0000 0000 0000 0000 0000 0000
#define CONTROL_MASK 0x40000000  // 0100 0000 0000 0000 0000 0000 0000 0000
#define KEY_MASK ~(SHIFT_MASK | CONTROL_MASK)

#define HAS_CTRL(k) (CONTROL_MASK == (((long)(k)) & CONTROL_MASK))
#define HAS_SHIFT(k) (SHIFT_MASK == (((long)(k)) & SHIFT_MASK))
#define GET_RAYKEY(k) ((k) & KEY_MASK)

typedef void (*InputCallback)(doko_control_t*);

typedef struct {
    unsigned long key;
    InputCallback function;
    doko_screen_t screen;
    double lastPressedTime;
    double keyTriggerRate;
} InputMapping;

void keybind_zoomInCenterImage(doko_control_t* ctrl);
void keybind_zoomOutCenterImage(doko_control_t* ctrl);
void keybind_zoomInMousePosition(doko_control_t* ctrl);
void keybind_zoomOutMousePosition(doko_control_t* ctrl);

void keybind_nextImage(doko_control_t* ctrl);
void keybind_PrevImage(doko_control_t* ctrl);

void keybind_printDebugInfo(doko_control_t* ctrl);

void keybind_moveImageUp(doko_control_t* ctrl);
void keybind_moveImageDown(doko_control_t* ctrl);
void keybind_moveImageLeft(doko_control_t* ctrl);
void keybind_moveImageRight(doko_control_t* ctrl);
void keybind_moveImageByMouseDelta(doko_control_t *ctrl);

void keybind_centerImage(doko_control_t* ctrl);
void keybind_fitCenterImage(doko_control_t* ctrl);

void keybind_flipVertical(doko_control_t* ctrl);
void keybind_flipHorizontal(doko_control_t* ctrl);

void keybind_colorInvert(doko_control_t* ctrl);
void keybind_colorInvertShader(doko_control_t* ctrl);
void keybind_colorGrayscaleShader(doko_control_t* ctrl);

void keybind_increaseFPS(doko_control_t* ctrl);
void keybind_decreaseFPS(doko_control_t* ctrl);

void keybind_cycleScreen(doko_control_t *ctrl);
void keybind_cycleScreenReverse(doko_control_t *ctrl);

#endif
