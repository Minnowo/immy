
// needed for popen
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "core.h"

// TODO: fix this

#if defined(__unix__) && defined(PLATFORM_LINUX)

int immy_paste_image_from_clipboard_linux(ImmyControl_t* ctrl) {

    L_D("%s: Pasting image", __func__);

    const char* CMD;

#if defined(DESKTOP_WAYLAND)

    CMD = WAYLAND_PASTE_IMAGE_COMMAND;

#elif defined(DESKTOP_X11)

    CMD = X11_PASTE_IMAGE_COMMAND;

#else

#error "We are PLATFORM_LINUX but not DESKTOP_WAYLAND or DESKTOP_X11!!!"

#endif

    // block until the command can get the clipboard data
    if (system(CMD) == -1) {

        L_E("%s: error saving clipboard to temp file: %s", __func__, strerror(errno));

        return false;
    }

    L_I("Clipboard data saved to " PASTE_COMMAND_OUTPUT_FILE, __func__);

    // add the image to the control
    return iAddImage(ctrl, PASTE_COMMAND_OUTPUT_FILE);
}

bool immy_copy_image_to_clipboard_linux(ImmyImage_t* im) {

    L_I("Copying image: %d x %d", im->rayim.width, im->rayim.height);

    const char* CMD;

#if defined(DESKTOP_WAYLAND)

    CMD = WAYLAND_COPY_IMAGE_COMMAND;

#elif defined(DESKTOP_X11)

    CMD = X11_COPY_IMAGE_COMMAND;

#else

#error "We are PLATFORM_LINUX but not DESKTOP_WAYLAND or DESKTOP_X11!!!"

#endif

    // we will write the image to stdin
    FILE* fp = popen(CMD , "w");

    if (!fp) {

        L_E("%s: popen failed: %s", __func__, strerror(errno));

        return false;
    }

    int filesize;

    // png is our only option just using raylib
    unsigned char* png_bytes = ExportImageToMemory(im->rayim, ".png", &filesize);

    if (!png_bytes) {

        L_E("%s: Could not write to png: %s", __func__, strerror(errno));

        return false;
    }

    L_D("Wrote png is %0.2fmb", BYTES_TO_MB(filesize));

    fwrite(png_bytes, 1, filesize, fp);

    pclose(fp);

    // make sure we free it properly
    RL_FREE(png_bytes);

    return true;
}

#endif

int iPasteImageFromClipboard(ImmyControl_t* ctrl) {

#if defined(__unix__) && defined(PLATFORM_LINUX)
    return immy_paste_image_from_clipboard_linux(ctrl);
#endif

    return false;
}

bool iCopyImageToClipboard(ImmyImage_t* im) {

#if defined(__unix__) && defined(PLATFORM_LINUX)
    return immy_copy_image_to_clipboard_linux(im);
#endif

    return false;
}
