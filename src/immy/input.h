

#ifndef IMMY_KEYBINDS_H
#define IMMY_KEYBINDS_H

#include "core/core.h"
#include <raylib.h>

typedef void (*InputCallback)(ImmyControl_t*);

typedef struct {

        unsigned int  key;             // key to trigger
        InputCallback function;        // the callback of the key
        UIScreen_t    screen;          // the screen the keybind works on
        double        lastPressedTime; // when the key was last pressed
        double        keyTriggerRate;  // how fast the key can be triggered
        unsigned int  keyIsWorth;      // how many keypress this adds to the limit
        const char*   NAME;            // the name for the keybinds page
} InputMapping;

#define BIND(k, func, scr, rate)                                                                                       \
    (InputMapping) {                                                                                                   \
        .key = (k), .function = (func), .screen = (scr), .lastPressedTime = 0, .keyTriggerRate = (rate),               \
        .keyIsWorth = 1, .NAME = ((#func) + 3)                                                                         \
    }
#define BINDX(k, func, scr, worth, rate)                                                                               \
    (InputMapping) {                                                                                                   \
        .key = (k), .function = (func), .screen = (scr), .lastPressedTime = 0, .keyTriggerRate = (rate),               \
        .keyIsWorth = (worth), .NAME = ((#func) + 3)                                                                   \
    }

/////////////
// The naming here is used to the keybind page.
// The first 3 characters are removed to show the string on the keybinds page.
/////////////

void kb_Zoom_In_Center_Image(ImmyControl_t* ctrl);
void kb_Zoom_Out_Center_Image(ImmyControl_t* ctrl);
void kb_Zoom_In_Mouse_Position(ImmyControl_t* ctrl);
void kb_Zoom_Out_Mouse_Position(ImmyControl_t* ctrl);

void kb_Next_Image_By_N(ImmyControl_t* ctrl, int by);
void kb_Prev_Image_By_N(ImmyControl_t* ctrl, int by);
void kb_Next_Image(ImmyControl_t* ctrl);
void kb_Prev_Image(ImmyControl_t* ctrl);
void kb_Next_Image_By_10(ImmyControl_t* ctrl);
void kb_Prev_Image_By_10(ImmyControl_t* ctrl);

void kb_Jump_Image_N(ImmyControl_t* ctrl, int to);
void kb_Jump_Image_Start(ImmyControl_t* ctrl);
void kb_Jump_Image_End(ImmyControl_t* ctrl);

void kb_Print_Debug_Info(ImmyControl_t* ctrl);

void kb_Move_Image_Up(ImmyControl_t* ctrl);
void kb_Move_Image_Down(ImmyControl_t* ctrl);
void kb_Move_Image_Left(ImmyControl_t* ctrl);
void kb_Move_Image_Right(ImmyControl_t* ctrl);
void kb_Move_Image_By_Mouse_Delta(ImmyControl_t* ctrl);

void kb_Center_Image(ImmyControl_t* ctrl);
void kb_Fit_Center_Image(ImmyControl_t* ctrl);

void kb_Flip_Vertical(ImmyControl_t* ctrl);
void kb_Flip_Horizontal(ImmyControl_t* ctrl);

void kb_Color_Invert(ImmyControl_t* ctrl);
void kb_Color_Invert_Shader(ImmyControl_t* ctrl);
void kb_Color_Grayscale_Shader(ImmyControl_t* ctrl);

void kb_Increase_FPS(ImmyControl_t* ctrl);
void kb_Decrease_FPS(ImmyControl_t* ctrl);

void kb_Toggle_Image_Filelist_Screen(ImmyControl_t* ctrl);
void kb_Toggle_Show_Bar(ImmyControl_t* ctrl);

void kb_Cycle_Screen(ImmyControl_t* ctrl);
void kb_Cycle_Screen_Reverse(ImmyControl_t* ctrl);

void kb_Goto_Image_Screen(ImmyControl_t* ctrl);
void kb_Goto_File_List_Screen(ImmyControl_t* ctrl);
void kb_Goto_Thumb_Screen(ImmyControl_t* ctrl);
void kb_Goto_Keybinds_Screen(ImmyControl_t* ctrl);

void kb_Scroll_Keybind_List_Up(ImmyControl_t* ctrl);
void kb_Scroll_Keybind_List_Down(ImmyControl_t* ctrl);

void kb_Copy_Image_To_Clipboard(ImmyControl_t* ctrl);
void kb_Paste_Image_From_Clipboard(ImmyControl_t* ctrl);

void kb_Dither(ImmyControl_t* ctrl);

void kb_Cycle_Image_Interpolation(ImmyControl_t* ctrl);

void kb_Thumb_Page_Down(ImmyControl_t* ctrl);
void kb_Thumb_Page_Up(ImmyControl_t* ctrl);
void kb_Thumb_Page_Left(ImmyControl_t* ctrl);
void kb_Thumb_Page_Right(ImmyControl_t* ctrl);

void kb_Quit(ImmyControl_t* ctrl);

void kb_Rotate_Image_Left(ImmyControl_t* ctrl);
void kb_Rotate_Image_Right(ImmyControl_t* ctrl);
#endif
