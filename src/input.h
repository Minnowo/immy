

#ifndef IMMY_KEYBINDS_H
#define IMMY_KEYBINDS_H

#include "core/core.h"
#include <raylib.h>


typedef void (*InputCallback)(immy_control_t*);

typedef struct {

    unsigned int  key;             // key to trigger
    InputCallback function;        // the callback of the key
    immy_screen_t screen;          // the screen the keybind works on
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

void kb_Zoom_In_Center_Image(immy_control_t* ctrl);
void kb_Zoom_Out_Center_Image(immy_control_t* ctrl);
void kb_Zoom_In_Mouse_Position(immy_control_t* ctrl);
void kb_Zoom_Out_Mouse_Position(immy_control_t* ctrl);

void kb_Next_Image_By_N(immy_control_t* ctrl, int by);
void kb_Prev_Image_By_N(immy_control_t* ctrl, int by);
void kb_Next_Image(immy_control_t* ctrl);
void kb_Prev_Image(immy_control_t* ctrl);
void kb_Next_Image_By_10(immy_control_t* ctrl);
void kb_Prev_Image_By_10(immy_control_t* ctrl);

void kb_Jump_Image_N(immy_control_t* ctrl, int to);
void kb_Jump_Image_Start(immy_control_t* ctrl);
void kb_Jump_Image_End(immy_control_t* ctrl);

void kb_Print_Debug_Info(immy_control_t* ctrl);

void kb_Move_Image_Up(immy_control_t* ctrl);
void kb_Move_Image_Down(immy_control_t* ctrl);
void kb_Move_Image_Left(immy_control_t* ctrl);
void kb_Move_Image_Right(immy_control_t* ctrl);
void kb_Move_Image_By_Mouse_Delta(immy_control_t* ctrl);

void kb_Center_Image(immy_control_t* ctrl);
void kb_Fit_Center_Image(immy_control_t* ctrl);

void kb_Flip_Vertical(immy_control_t* ctrl);
void kb_Flip_Horizontal(immy_control_t* ctrl);

void kb_Color_Invert(immy_control_t* ctrl);
void kb_Color_Invert_Shader(immy_control_t* ctrl);
void kb_Color_Grayscale_Shader(immy_control_t* ctrl);

void kb_Increase_FPS(immy_control_t* ctrl);
void kb_Decrease_FPS(immy_control_t* ctrl);

void kb_Toggle_Image_Filelist_Screen(immy_control_t* ctrl);
void kb_Toggle_Show_Bar(immy_control_t* ctrl);

void kb_Cycle_Screen(immy_control_t* ctrl);
void kb_Cycle_Screen_Reverse(immy_control_t* ctrl);

void kb_Goto_Image_Screen(immy_control_t* ctrl);
void kb_Goto_File_List_Screen(immy_control_t* ctrl);
void kb_Goto_Thumb_Screen(immy_control_t* ctrl);
void kb_Goto_Keybinds_Screen(immy_control_t* ctrl);

void kb_Scroll_Keybind_List_Up(immy_control_t* ctrl);
void kb_Scroll_Keybind_List_Down(immy_control_t* ctrl);

void kb_Scroll_Thumb_Page_Up(immy_control_t* ctrl);
void kb_Scroll_Thumb_Page_Down(immy_control_t* ctrl);

void kb_Copy_Image_To_Clipboard(immy_control_t* ctrl);
void kb_Paste_Image_From_Clipboard(immy_control_t* ctrl);

void kb_Dither(immy_control_t* ctrl);

void kb_Cycle_Image_Interpolation(immy_control_t* ctrl);

void kb_Thumb_Page_Down(immy_control_t* ctrl);
void kb_Thumb_Page_Up(immy_control_t* ctrl);
void kb_Thumb_Page_Left(immy_control_t* ctrl);
void kb_Thumb_Page_Right(immy_control_t* ctrl);

#endif
