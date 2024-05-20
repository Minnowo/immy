
#include "input.h"
#include "raylib.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "darray.h"
#include "doko.h"
#include "ui.h"

void* display;
struct doko_control this;

static inline void sort_file_list(FilePathList fpl) {

    switch (this.filename_cmp) {

    case SORT_ORDER__DEFAULT:
        qsort(fpl.paths, fpl.count, sizeof(fpl.paths[0]), doko_qsort_strcmp);
        break;

    case SORT_ORDER__NATURAL:
        qsort(fpl.paths, fpl.count, sizeof(fpl.paths[0]), doko_qsort_natstrcmp);
        break;
    }
}

void do_override_redirect() {

    /*
    #include <GLFW/glfw3.h>
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>

    display = XOpenDisplay(NULL);

    if (!display) {

        L_E("Could not open X11 display: %s", strerror(errno));

        return;
    }

    // // we don't have this glfwGetX11Window function
    // // the raylib GetWindowHandle function does not return the x11 handle
    Window window =  glfwGetX11Window(GetWindowHandle());

    XSetWindowAttributes attrs;
    attrs.override_redirect = 1;

    int i = XChangeWindowAttributes(display, window, CWOverrideRedirect,
    &attrs);

    L_E("XChangeWindowAttributes %d", i);

    XUnmapWindow(display, window);
    XMapWindow(display, window);
    */
}

void add_file(const char* path_) {

    char* path = doko_strdup(path_);

    if (!path) {
        L_E("Cannot duplicate string '%s'! %s", strerror(errno));
        return;
    }

    L_D("Adding file %s", path);

    const char* name = GetFileName(path);

    doko_image_t i = {
        .path        = path,
        .name        = name,
        .rayim       = {0},
        .scale       = 1,
        .rotation    = 0,
        .rebuildBuff = 0,
        .status      = IMAGE_STATUS_NOT_LOADED,
        .srcRect     = {0},
        .dstPos      = {0},
    };

    int setim = this.image_files.size == 0;

    DARRAY_APPEND(this.image_files, i);

    if (setim) {
        set_image(&this, 0);
    }
}

void handle_dropped_files() {

    FilePathList fpl = LoadDroppedFiles();

    sort_file_list(fpl);

    for (size_t i = 0; i < fpl.count; i++) {
        add_file(fpl.paths[i]);
    }

    UnloadDroppedFiles(fpl);
}

void do_mouse_input() {

    int    s    = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    int    c    = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    int    kc   = 0;
    double time = GetTime();

    for (size_t i = 0; i < MOUSEBIND_COUNT; ++i) {

        // clang-format off
        if (

            (this.screen != mousebinds[i].screen && 
             mousebinds[i].screen != DOKO_SCREEN__ALL) ||

            (c != HAS_CTRL(mousebinds[i].key)) ||
            (s != HAS_SHIFT(mousebinds[i].key)) ||

            (time - mousebinds[i].lastPressedTime < mousebinds[i].keyTriggerRate) ||

            !(GetMouseWheelMove() > 0 && GET_RAYKEY(mousebinds[i].key) == MOUSE_WHEEL_FWD) &&
            !(GetMouseWheelMove() < 0 && GET_RAYKEY(mousebinds[i].key) == MOUSE_WHEEL_BWD) &&
            !(IsMouseButtonDown(GET_RAYKEY(mousebinds[i].key)))
            ) {
            continue;
        }
        // clang-format on

        mousebinds[i].function(&this);
        mousebinds[i].lastPressedTime = GetTime();
        this.renderFrames             = RENDER_FRAMES;

        if (++kc == MOUSE_LIMIT) {
            return;
        }
    }
}

void do_keyboard_input() {

    int    s    = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    int    c    = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    int    kc   = 0;
    double time = GetTime();

    for (size_t i = 0; i < KEYBIND_COUNT; ++i) {

        // clang-format off
        if (
            (this.screen != keybinds[i].screen &&
             keybinds[i].screen != DOKO_SCREEN__ALL) ||

            (time - keybinds[i].lastPressedTime < keybinds[i].keyTriggerRate) ||

            (c != HAS_CTRL(keybinds[i].key)) ||
            (s != HAS_SHIFT(keybinds[i].key)) ||

            !IsKeyDown(GET_RAYKEY(keybinds[i].key))
            ) {
            continue;
        }
        // clang-format on

        keybinds[i].function(&this);
        keybinds[i].lastPressedTime = time;
        this.renderFrames           = RENDER_FRAMES;

        if (++kc == KEY_LIMIT) {
            return;
        }
    }
}

// returns the number of arguments to skip
int handle_flags(
    doko_config_t* config, const char* flag_str, const char* flag_value
) {

    size_t flen = strlen(flag_str);

    if (flen < 2 || flag_str[0] != '-')
        return 0;

    for (size_t i = 1; i < flen; ++i)

        switch (flag_str[i]) {

        case 'f':
            config->window_flags |=
                FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_TOPMOST;
            continue;

        case 'B':
            config->show_bar = false;
            info_bar_height  = 0;
            continue;

        case 'b':
            config->show_bar = true;
            info_bar_height  = INFO_BAR_HEIGHT;
            continue;

        case 'D':
            config->terminal = true;
            continue;

        case 'd':
            config->terminal = false;
            continue;

        case 'C':
            config->center_image_on_start = false;
            continue;

        case 'c':
            config->center_image_on_start = true;
            continue;

        case 't':

            DIE_IF_NULL(flag_value, CLI_HELP_t_FLAG);

            config->window_title = flag_value;
            return 1;

        case 'x':

            DIE_IF_NULL(flag_value, CLI_HELP_x_FLAG);

            config->window_x         = atoi(flag_value);
            config->set_win_position = true;
            return 1;

        case 'y':

            DIE_IF_NULL(flag_value, CLI_HELP_y_FLAG);

            config->window_y         = atoi(flag_value);
            config->set_win_position = true;
            return 1;

        case 'w':

            DIE_IF_NULL(flag_value, CLI_HELP_w_FLAG);

            config->window_width = atoi(flag_value);

            if (config->window_width == 0)
                DIE(CLI_HELP_w_FLAG);

            return 1;

        case 'h':

            DIE_IF_NULL(flag_value, CLI_HELP_h_FLAG);

            config->window_height = atoi(flag_value);

            if (config->window_height == 0)
                DIE(CLI_HELP_h_FLAG);

            return 1;

        case 'l':

            DIE_IF_NULL(flag_value, CLI_HELP_l_FLAG);

            log_level = atoi(flag_value);

            if (log_level < LOG_LEVEL_DEBUG || log_level > LOG_LEVEL_NOTHING)
                DIE(CLI_HELP_l_FLAG);

            return 1;
        }

    return 0;
}

void handle_start_args(doko_config_t* config, int argc, char* argv[]) {

    for (int i = 1; i < argc; i++) {

        if (!FileExists(argv[i])) {

            if (i + 1 < argc) {
                i += handle_flags(config, argv[i], argv[i + 1]);
            } else {
                i += handle_flags(config, argv[i], NULL);
            }

            continue;
        }

        if (!DirectoryExists(argv[i])) {

            add_file(argv[i]);
            continue;
        }

        L_I("Scanning directory %s for files", argv[i]);

        FilePathList fpl = LoadDirectoryFilesEx(
            argv[i], IMAGE_FILE_FILTER, SEARCH_DIRS_RECURSIVE
        );

        sort_file_list(fpl);

        for (size_t j = 0; j < fpl.count; j++) {
            add_file(fpl.paths[j]);
        }

        UnloadDirectoryFiles(fpl);
    }
}

void detach_from_terminal() {

#if (ALLOW_DETACH_FROM_TERMINAL == 1)
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
#endif
}

int main(int argc, char* argv[]) {
    memset(&this, 0, sizeof(this));

    this.config.window_title          = WINDOW_TITLE;
    this.config.window_x              = 0;
    this.config.window_y              = 0;
    this.config.set_win_position      = false;
    this.config.window_width          = START_WIDTH;
    this.config.window_height         = START_HEIGHT;
    this.config.window_min_width      = MIN_WINDOW_WIDTH;
    this.config.window_min_height     = MIN_WINDOW_HEIGHT;
    this.config.window_flags          = FLAG_WINDOW_RESIZABLE;
    this.config.center_image_on_start = CENTER_IMAGE_ON_FIRST_START;
    this.config.terminal              = !DETACH_FROM_TERMINAL;
    this.config.show_bar              = true;

    this.filename_cmp = DEFAULT_SORT_ORDER;

    handle_start_args(&this.config, argc, argv);

    if (this.image_files.size <= 0)
        DIE("no arguments given\n\n" CLI_HELP);

    this.selected_image = this.image_files.buffer;
    this.renderFrames   = RENDER_FRAMES;

    // we want to load the image before we lose the terminal
    // so that scripts know we have the image and can remove it
    if (this.selected_image != NULL)
        doko_loadImage(this.selected_image);

    if (!this.config.terminal)
        detach_from_terminal();

    ui_init(&this.config);

    do_override_redirect();

    ui_loadCodepointsFromFileList(&this);

    // this loads the image and makes sure it is actually center
    // since my tiling wm spawns the window floating and then unfloats it
    if (this.selected_image != NULL && this.config.center_image_on_start)

        for (int i = 0; i < 2; i++) {
            BeginDrawing();
            ui_renderImage(this.selected_image);
            kb_Fit_Center_Image(&this);
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

            case DOKO_SCREEN_KEYBINDS:

                ui_renderKeybinds(&this);
                break;

            case DOKO_SCREEN_FILE_LIST:
                ui_renderFileList(&this);
                break;

            case DOKO_SCREEN_IMAGE:

                if (this.selected_image == NULL) {
                    ui_renderTextOnInfoBar(
                        "There is no image selected! Drag an image to view."
                    );
                    break;
                }

                ui_renderImage(this.selected_image);
                ui_renderPixelGrid(this.selected_image);

                if (this.message.message != NULL &&
                    this.message.show_for_frames > 0) {

                    this.message.show_for_frames--;

                    if (this.config.show_bar)
                        ui_renderTextOnInfoBar(this.message.message);

                    if (this.message.free_when_done &&
                        this.message.show_for_frames == 0) {

                        free(this.message.message);

                        this.message.message = NULL;
                    }

                } else if (this.config.show_bar) {

                    ui_renderInfoBar(this.selected_image);
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

        if (im.status == IMAGE_STATUS_LOADED) {
            UnloadImage(im.rayim);
        }

        if (im.path != NULL) {
            free(im.path);
        }
    }

    DARRAY_FREE(this.image_files);

    ui_deinit();

    return 0;
}
