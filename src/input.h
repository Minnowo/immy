

#ifndef DOKO_KEYBINDS_H
#define DOKO_KEYBINDS_H

#include "doko.h"
#include <raylib.h>

#define SHIFT_MASK 0x80000000   // 1000 0000 0000 0000 0000 0000 0000 0000
#define CONTROL_MASK 0x40000000 // 0100 0000 0000 0000 0000 0000 0000 0000
#define KEY_MASK ~(SHIFT_MASK | CONTROL_MASK)

#define HAS_CTRL(k) ((((int)(k)) & CONTROL_MASK) != 0)
#define HAS_SHIFT(k) ((((int)(k)) & SHIFT_MASK) != 0)
#define GET_RAYKEY(k) ((k) & KEY_MASK)

typedef void (*InputCallback)(doko_control_t*);

typedef struct {

    unsigned int  key;             // key to trigger
    InputCallback function;        // the callback of the key
    doko_screen_t screen;          // the screen the keybind works on
    double        lastPressedTime; // when the key was last pressed
    double        keyTriggerRate;  // how fast the key can be triggered
    unsigned int  keyIsWorth;      // how many keypress this adds to the limit
    const char*   NAME;            // the name for the keybinds page
} InputMapping;

#define BIND(k, func, scr, rate)                                               \
    (InputMapping) {                                                           \
        .key = (k), .function = (func), .screen = (scr), .lastPressedTime = 0, \
        .keyTriggerRate = (rate), .keyIsWorth = 1, .NAME = ((#func) + 3)       \
    }
#define BINDX(k, func, scr, worth, rate)                                       \
    (InputMapping) {                                                           \
        .key = (k), .function = (func), .screen = (scr), .lastPressedTime = 0, \
        .keyTriggerRate = (rate), .keyIsWorth = (worth), .NAME = ((#func) + 3) \
    }

void kb_Zoom_In_Center_Image(doko_control_t* ctrl);
void kb_Zoom_Out_Center_Image(doko_control_t* ctrl);
void kb_Zoom_In_Mouse_Position(doko_control_t* ctrl);
void kb_Zoom_Out_Mouse_Position(doko_control_t* ctrl);

void kb_Next_Image_By_N(doko_control_t* ctrl, int by);
void kb_Prev_Image_By_N(doko_control_t* ctrl, int by);
void kb_Next_Image(doko_control_t* ctrl);
void kb_Prev_Image(doko_control_t* ctrl);
void kb_Next_Image_By_10(doko_control_t* ctrl);
void kb_Prev_Image_By_10(doko_control_t* ctrl);

void kb_Jump_Image_N(doko_control_t* ctrl, int to);
void kb_Jump_Image_Start(doko_control_t* ctrl);
void kb_Jump_Image_End(doko_control_t* ctrl);

void kb_Print_Debug_Info(doko_control_t* ctrl);

void kb_Move_Image_Up(doko_control_t* ctrl);
void kb_Move_Image_Down(doko_control_t* ctrl);
void kb_Move_Image_Left(doko_control_t* ctrl);
void kb_Move_Image_Right(doko_control_t* ctrl);
void kb_Move_Image_By_Mouse_Delta(doko_control_t* ctrl);

void kb_Center_Image(doko_control_t* ctrl);
void kb_Fit_Center_Image(doko_control_t* ctrl);

void kb_Flip_Vertical(doko_control_t* ctrl);
void kb_Flip_Horizontal(doko_control_t* ctrl);

void kb_Color_Invert(doko_control_t* ctrl);
void kb_Color_Invert_Shader(doko_control_t* ctrl);
void kb_Color_Grayscale_Shader(doko_control_t* ctrl);

void kb_Increase_FPS(doko_control_t* ctrl);
void kb_Decrease_FPS(doko_control_t* ctrl);

void kb_Toggle_Image_Filelist_Screen(doko_control_t* ctrl);
void kb_Toggle_Show_Bar(doko_control_t* ctrl);

void kb_Cycle_Screen(doko_control_t* ctrl);
void kb_Cycle_Screen_Reverse(doko_control_t* ctrl);

void kb_Goto_Image_Screen(doko_control_t* ctrl);
void kb_Goto_File_List_Screen(doko_control_t* ctrl);
void kb_Goto_Thumb_Screen(doko_control_t* ctrl);
void kb_Goto_Keybinds_Screen(doko_control_t* ctrl);

void kb_Scroll_Keybind_List_Up(doko_control_t* ctrl);
void kb_Scroll_Keybind_List_Down(doko_control_t* ctrl);

void kb_Scroll_Thumb_Page_Up(doko_control_t* ctrl);
void kb_Scroll_Thumb_Page_Down(doko_control_t* ctrl);

void kb_Copy_Image_To_Clipboard(doko_control_t* ctrl);
void kb_Paste_Image_From_Clipboard(doko_control_t* ctrl);

void kb_Dither(doko_control_t* ctrl);

void kb_Cycle_Image_Interpolation(doko_control_t* ctrl);

#endif
