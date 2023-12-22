
#include "input.h"
#include "raylib.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "darray.h"
#include "ui.h"
#include "config.h"
#include "doko.h"

struct doko_control this;


void add_file(const char* path_) {

    char *path = doko_strdup(path_);

    if (!path) {
        return doko_error(EXIT_FAILURE, errno,
                          "Cannot duplicate str '%s'! out of memory.\n", path_);
    }

    const char* _= GetFileName(path_);

    doko_image_t i = {
        .path = path,
        .nameOffset = _ - path_,
        .rayim = NULL,
        .scale = 1,
        .rotation = 0,
        .rebuildBuff = 0,
        .status = IMAGE_STATUS_NOT_LOADED,
        .srcRect = 0,
        .dstPos = 0,
    };

    int setim = this.image_files.size == 0;

    DARRAY_APPEND(this.image_files, i);

    if(setim) {
        set_image(&this, 0);
    }
}

#ifdef ENABLE_FILE_DROP

void handle_dropped_files() {

    FilePathList fpl = LoadDroppedFiles();

    for (size_t i = 0; i < fpl.count; i++) {
        add_file(fpl.paths[i]);
    }

    UnloadDroppedFiles(fpl);
}

#endif

#ifdef ENABLE_MOUSE_INPUT

void do_mouse_input() {

    int s = !!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
    int c = !!(IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL));

    for (size_t i = 0, kc = 0; i < MOUSEBIND_COUNT; ++i) {

        if (!(c == !!(mousebinds[i].key & CONTROL_MASK)) ||
            !(s == !!(mousebinds[i].key & SHIFT_MASK))) {
            continue;
        }

        if (GetMouseWheelMove() > 0 && (mousebinds[i].key & KEY_MASK) == MOUSE_WHEEL_FORWARD_BUTTON) {
            mousebinds[i].function(&this);
            ++this.renderFrames;
            continue;
        }

        if(GetMouseWheelMove() < 0 &&  (mousebinds[i].key & KEY_MASK) == MOUSE_WHEEL_BACKWARD_BUTTON ) {
            mousebinds[i].function(&this);
            ++this.renderFrames;
            continue;
        }

        if (!IsMouseButtonDown(mousebinds[i].key & KEY_MASK)) {
            continue;
        }

        mousebinds[i].function(&this);
        ++this.renderFrames;

        if(++kc == MOUSE_LIMIT) {
            return;
        }
    }
}

#endif

#ifdef ENABLE_KEYBOARD_INPUT

void do_keyboard_input() {

#define _DO_KEYBOARD_INPUT(map, size)                                          \
    do {                                                                       \
        int s = !!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));   \
        int c =                                                                \
            !!(IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL));   \
                                                                               \
        for (size_t i = 0, kc = 0; i < (size); ++i) {                          \
            if (!(c == !!((map)[i].key & CONTROL_MASK)) ||                     \
                !(s == !!((map)[i].key & SHIFT_MASK)) ||                       \
                !IsKeyDown((map)[i].key & KEY_MASK)) {                         \
                continue;                                                      \
            }                                                                  \
                                                                               \
            (map)[i].function(&this);                                          \
            this.keyPressedTime = GetTime();                                   \
            ++this.renderFrames;                                               \
                                                                               \
            if (++kc == KEY_LIMIT) {                                           \
                return;                                                        \
            }                                                                  \
        }                                                                      \
    } while (0)

#ifdef ENABLE_KEYBOARD_INPUT_NO_DELAY
    _DO_KEYBOARD_INPUT(keybinds_no_time_limit, KEYBIND_NO_LIMIT_COUNT);
#endif

    if ((GetTime() - this.keyPressedTime < KEY_TIME_LIMIT)) {
        return;
    }

    _DO_KEYBOARD_INPUT(keybinds, KEYBIND_COUNT);
}

#endif

void handle_start_args(int argc, char* argv[]) {

    for (int i = 1; i < argc; i++) {

        if (!FileExists(argv[i])) {
            continue;
        }

        if (!DirectoryExists(argv[i])) {

            add_file(argv[i]);
            continue;
        }

        FilePathList fpl = LoadDirectoryFilesEx(argv[i], IMAGE_FILE_FILTER,
                                                SEARCH_DIRS_RECURSIVE);

        for (size_t j = 0; j < fpl.count; j++) {
            add_file(fpl.paths[j]);
        }

        UnloadDirectoryFiles(fpl);
    }
}

int main(int argc, char* argv[])
{
    // if(argc == 1) {
    //     doko_error(EXIT_FAILURE, errno, "No start arguments given.");
    //     return 1;
    // }

    memset(&this, 0, sizeof(this));

    handle_start_args(argc, argv);

    // if(this.image_files.size == 0) {
    //     doko_error(EXIT_FAILURE, errno, "No files given.");
    //     return 1;
    // }

    if(this.image_files.size > 0) {
        this.selected_image = this.image_files.buffer;
    }
    this.renderFrames = 5;

    ui_init();


    while (!WindowShouldClose()) {

        ++this.frame;

#ifdef ENABLE_FILE_DROP
        if (IsFileDropped()) {

            handle_dropped_files();
        }
#endif

#ifdef ENABLE_KEYBOARD_INPUT
        do_keyboard_input();
#endif

#ifdef ENABLE_MOUSE_INPUT
        do_mouse_input();
#endif

#if defined(ENABLE_MOUSE_INPUT) || defined(ENABLE_KEYBOARD_INPUT)
        this.lastMouseClick = GetMousePosition();
#endif

        BeginDrawing();

#ifndef ALWAYS_DO_RENDER
        if (this.renderFrames > 0 || IsWindowResized() ||
            !(this.frame % REDRAW_ON_FRAME)) {

#endif
            ui_renderBackground();

            if (this.selected_image != NULL) {

                ui_renderImage(this.selected_image);
                ui_renderPixelGrid(this.selected_image);
                ui_renderInfoBar(this.selected_image);
            } else {
                ui_renderTextOnInfoBar(
                    "There is no image selected! Drag an image to view.");
            }

            DrawFPS(0, 0);

            this.renderFrames -= 1 - (this.renderFrames <= 0);

#ifndef ALWAYS_DO_RENDER
        }
#endif

        EndDrawing();
    }

    DARRAY_FOR_EACH_I(this.image_files, i) {

        doko_image_t im = this.image_files.buffer[i];

        if(im.status == IMAGE_STATUS_LOADED) {
            UnloadImage(im.rayim);
        }

        if(im.path != NULL) {
            free(im.path);
        }
    }

    DARRAY_FREE(this.image_files);

    ui_deinit();

    return 0;
}