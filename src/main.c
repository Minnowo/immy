
#include "input.h"
#include "raylib.h"


#include <stdio.h>
#include <unistd.h>
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

static inline void sort_file_list(FilePathList fpl){

    switch(this.filename_cmp){
        case SORT_ORDER__DEFAULT:
            qsort(fpl.paths, fpl.count, sizeof(fpl.paths[0]), doko_qsort_strcmp);
            break;
        case SORT_ORDER__NATURAL:
            qsort(fpl.paths, fpl.count, sizeof(fpl.paths[0]), doko_qsort_natstrcmp);
            break;
    }
}

void add_file(const char* path_) {

    char *path = doko_strdup(path_);

    if (!path) {
        L_E("Cannot duplicate string '%s'! %s", strerror(errno));
        return;
    }

    L_D("Adding file %s", path);

    const char *name = GetFileName(path);

    doko_image_t i = {
        .path = path,
        .name = name,
        .rayim = {0},
        .scale = 1,
        .rotation = 0,
        .rebuildBuff = 0,
        .status = IMAGE_STATUS_NOT_LOADED,
        .srcRect = {0},
        .dstPos = {0},
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

    sort_file_list(fpl);

    for (size_t i = 0; i < fpl.count; i++) {
        add_file(fpl.paths[i]);
    }

    UnloadDroppedFiles(fpl);
}

#endif

#ifdef ENABLE_MOUSE_INPUT

void do_mouse_input() {

    int s = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    int c = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    double time = GetTime();

    for (size_t i = 0, kc = 0; i < MOUSEBIND_COUNT; ++i) {

        if (!(c == HAS_CTRL(mousebinds[i].key)) ||
            !(s == HAS_SHIFT(mousebinds[i].key)) || 
            time - mousebinds[i].lastPressedTime < mousebinds[i].keyTriggerRate
            ) {
            continue;
        }

        if (GetMouseWheelMove() > 0 && GET_RAYKEY(mousebinds[i].key) == MOUSE_WHEEL_FORWARD_BUTTON) { 
            // fallthrough
        } 
        else if (GetMouseWheelMove() < 0 && GET_RAYKEY(mousebinds[i].key) == MOUSE_WHEEL_BACKWARD_BUTTON) { 
            // fallthrough
        } 
        else if (!IsMouseButtonDown(GET_RAYKEY(mousebinds[i].key))) {
            continue;
        }

        mousebinds[i].function(&this);
        mousebinds[i].lastPressedTime = GetTime();
        this.renderFrames = RENDER_FRAMES;

        if(++kc == MOUSE_LIMIT) {
            return;
        }
    }
}

#endif

#ifdef ENABLE_KEYBOARD_INPUT

void do_keyboard_input() {


    int s = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    int c = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    double time = GetTime();

    for (size_t i = 0, kc = 0; i < KEYBIND_COUNT; ++i) {

        if (time - keybinds[i].lastPressedTime < keybinds[i].keyTriggerRate ||
            (this.screen != keybinds[i].screen && keybinds[i].screen != DOKO_SCREEN__ALL) ||
            !(c == HAS_CTRL(keybinds[i].key)) ||
            !(s == HAS_SHIFT(keybinds[i].key)) ||
            !IsKeyDown(GET_RAYKEY(keybinds[i].key))) {
            continue;
        }

        keybinds[i].function(&this);
        keybinds[i].lastPressedTime = GetTime();
        this.renderFrames = RENDER_FRAMES;

        if (++kc == KEY_LIMIT) {
            return;
        }
    }
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

        L_I("Scanning directory %s for files", argv[i]);

        FilePathList fpl = LoadDirectoryFilesEx(argv[i], IMAGE_FILE_FILTER,
                                                SEARCH_DIRS_RECURSIVE);

        sort_file_list(fpl);

        for (size_t j = 0; j < fpl.count; j++) {
            add_file(fpl.paths[j]);
        }

        UnloadDirectoryFiles(fpl);
    }
}

void detach_from_terminal() {

#ifdef __unix__

    int pid = fork();

    if (pid > 0) {
        exit(EXIT_SUCCESS);
        return;
    }

    if (pid < 0) {
        L_E("Could not fork process");
        return;
    }

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

#endif
}

int main(int argc, char* argv[])
{

#if (DETACH_FROM_TERMINAL == 1)
    detach_from_terminal();
#endif

    memset(&this, 0, sizeof(this));

    this.filename_cmp = DEFAULT_SORT_ORDER;

    handle_start_args(argc, argv);

    if(this.image_files.size > 0) {
        this.selected_image = this.image_files.buffer;
    }
    this.renderFrames = RENDER_FRAMES;

    ui_init();

    ui_loadCodepointsFromFileList(&this);


    // this loads the image and makes sure it is actually center
    // since my tiling wm spawns the window floating and then unfloats it
    if (this.selected_image != NULL)

        for(int i = 0; i < 2; i++){
            BeginDrawing();
            ui_renderImage(this.selected_image);
            keybind_fitCenterImage(&this);
            EndDrawing();
        }


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

        if (IsWindowResized() || this.frame % REDRAW_ON_FRAME == 0) {
            this.renderFrames = RENDER_FRAMES;
        }

        if (this.renderFrames > 0) {

#endif
            ui_renderBackground();

            switch (this.screen) {

                case DOKO_SCREEN__ALL:
                    break;

                case DOKO_SCREEN_FILE_LIST:
                    ui_renderFileList(&this);
                    break;

                case DOKO_SCREEN_IMAGE:
                    if (this.selected_image != NULL) {

                        ui_renderImage(this.selected_image);
                        ui_renderPixelGrid(this.selected_image);
                        ui_renderInfoBar(this.selected_image);
                    } else {
                        ui_renderTextOnInfoBar(
                            "There is no image selected! Drag an image to view.");
                    }
                    break;
            }

#if DRAW_FPS == 1
            DrawFPS(0, 0);
#endif

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
