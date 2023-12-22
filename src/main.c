
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

    DARRAY_APPEND(this.image_files, i);
}

void handle_dropped_files() {

    FilePathList fpl = LoadDroppedFiles();

    for (size_t i = 0; i < fpl.count; i++) {
        add_file(fpl.paths[i]);
    }

    UnloadDroppedFiles(fpl);
}

void do_input() {

    if ((GetTime() - this.keyPressedTime < KEY_TIME_LIMIT)) {
        return;
    }

    int s = !!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
    int c = !!(IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL));

    for (size_t i = 0, kc = 0; i < KEYBIND_COUNT; ++i) {

        if (!(c == !!(keybinds[i].key & CONTROL_MASK)) ||
            !(s == !!(keybinds[i].key & SHIFT_MASK) ) ||
            !IsKeyDown(keybinds[i].key & KEY_MASK)) {
            continue;
        }

        keybinds[i].function(&this);
        this.keyPressedTime = GetTime();
        ++this.renderFrames;

        if(++kc == KEY_LIMIT) {
            return;
        }
    }
}

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
    if(argc == 1) {
        doko_error(EXIT_FAILURE, errno, "No start arguments given.");
        return 1;
    }

    memset(&this, 0, sizeof(this));

    handle_start_args(argc, argv);

    if(this.image_files.size == 0) {
        doko_error(EXIT_FAILURE, errno, "No files given.");
        return 1;
    }

    this.renderFrames = 5;

    ui_init();

    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_Q)) {
            break;
        }

        if (IsFileDropped()) {

            handle_dropped_files();
        }

        do_input();

        BeginDrawing();

        if (this.renderFrames > 0 || IsWindowResized()) {

            ui_renderBackground();
            ui_renderImage(this.image_files.buffer + this.selected_file);
            ui_renderPixelGrid(this.image_files.buffer + this.selected_file);
            ui_renderInfoBar(this.image_files.buffer + this.selected_file);
            DrawFPS(0, 0);

            --this.renderFrames;
        }

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