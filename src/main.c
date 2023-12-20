
#include "raylib.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "darray.h"
#include "ui.h"
#include "config.h"
#include "doko.h"

dimage_arr_t image_files = {0};
size_t selected_file = 0;

float keyPressedTime = 0.0;

void add_file(const char* path_) {

    char *path = doko_strdup(path_);

    if (!path) {
        return doko_error(EXIT_FAILURE, errno,
                          "Cannot duplicate str '%s'! out of memory.\n", path_);
    }

    char* _= GetFileName(path_);

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

    DARRAY_APPEND(image_files, i);

    printf("Adding file %s to the list, filename is %s\n", path,
           i.path + i.nameOffset);
    printf("Files list now has %zu files with total %zu\n", image_files.size, image_files.length);
}

void handle_dropped_files() {

    FilePathList fpl = LoadDroppedFiles();

    for (size_t i = 0; i < fpl.count; i++) {
        add_file(fpl.paths[i]);
    }

    UnloadDroppedFiles(fpl);
}

void do_input() {

    doko_image_t *im = image_files.buffer + selected_file;

    if ((GetTime() - keyPressedTime >= KEY_TIME_LIMIT)) {

        keyPressedTime = GetTime();

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            if (IsKeyDown(KEY_K)) {
                doko_zoomImageOnPointFromClosest(im,true, GetScreenWidth() / 2,
                                      GetScreenHeight() / 2);
            } else if (IsKeyDown(KEY_J)) {
                doko_zoomImageOnPointFromClosest(im, false, GetScreenWidth() / 2,
                                      GetScreenHeight() / 2);
            } else if (IsKeyDown(KEY_L)) {
                selected_file = (selected_file + 1) % image_files.size;
            } else if (IsKeyDown(KEY_H)) {
                selected_file = (selected_file + image_files.size - 1) % image_files.size;
            } else {
                keyPressedTime = 0;
            }
        } 
        else if(IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyDown(KEY_H)) {
            } else if (IsKeyDown(KEY_L)) {
            } else {
                keyPressedTime = 0;
            }
        }
        else {
            if (IsKeyDown(KEY_I)) {

                DARRAY_FOR_EACH_I(image_files, i) {

                    doko_image_t t = image_files.buffer[i];

                    printf("%zu: %s\n", i, t.path);
                }
                printf("Image[%zu]:\n", selected_file);
                printf("   Size     %0.1f %0.1f\n", im->srcRect.width, im->srcRect.height);
                printf("   Visible  %0.1f %0.1f\n", im->srcRect.width * im->scale, im->srcRect.height * im->scale);
                printf("   Position %0.1f %0.1f\n", im->dstPos.x, im->dstPos.y);
                printf("   Scale %f\n", im->scale);

            } else if (IsKeyDown(KEY_K)) {
                doko_moveScrFracImage(im, 0, -1 / 5.0);
            } else if (IsKeyDown(KEY_J)) {
                doko_moveScrFracImage(im, 0, 1 / 5.0);
            } else if (IsKeyDown(KEY_H)) {
                doko_moveScrFracImage(im, -1 / 5.0, 0);
            } else if (IsKeyDown(KEY_L)) {
                doko_moveScrFracImage(im, 1 / 5.0, 0);
            } else if (IsKeyDown(KEY_C)) {
                doko_centerImage(im);
            } else if (IsKeyDown(KEY_M)) {
                ImageFlipHorizontal(&im->rayim);
                im->rebuildBuff = 1;
            } else if (IsKeyDown(KEY_V)) {
                ImageFlipVertical(&im->rayim);
                im->rebuildBuff = 1;
            } else if (IsKeyDown(KEY_U)) {
                ImageColorInvert(&im->rayim);
                im->rebuildBuff = 1;
            } else {
                keyPressedTime = 0;
            }
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

    handle_start_args(argc, argv);

    if(image_files.size == 0) {
        doko_error(EXIT_FAILURE, errno, "No files given.");
        return 1;
    }

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

        ui_renderBackground();
        ui_renderImage(image_files.buffer + selected_file);
        ui_renderPixelGrid(image_files.buffer + selected_file);
        ui_renderInfoBar(image_files.buffer + selected_file);

        EndDrawing();
    }

    DARRAY_FOR_EACH_I(image_files, i) {

        doko_image_t im = image_files.buffer[i];

        if(im.status == IMAGE_STATUS_LOADED) {
            UnloadImage(im.rayim);
        }

        if(im.path != NULL) {
            free(im.path);
        }
    }

    DARRAY_FREE(image_files);

    ui_deinit();

    return 0;
}