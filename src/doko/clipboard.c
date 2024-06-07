
// needed for popen
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../external/strnatcmp.h"
#include "doko.h"

// TODO: fix this

#ifdef __unix__

int doko_paste_image_from_clipboard_x11(doko_control_t* ctrl) {

    L_I("%s: Pasting image", __func__);

    // block until the command can get the clipboard data
    if (system(X11_PASTE_IMAGE_COMMAND) == -1) {

        L_E("%s: error saving clipboard to temp file: %s", __func__,
            strerror(errno));

        return false;
    }

    L_I("%s: Clipboard data saved to " X11_PASTE_COMMAND_OUTPUT_FILE, __func__);

    // add the image to the control
    return doko_add_image(ctrl, X11_PASTE_COMMAND_OUTPUT_FILE);
}

bool doko_copy_image_to_clipboard_x11(doko_image_t* im) {

    L_I("%s: Copying image: %d x %d", __func__, im->rayim.width,
        im->rayim.height);

    // we will write the image to stdin
    FILE* fp = popen(X11_COPY_IMAGE_COMMAND, "w");

    if (!fp) {

        L_E("%s: popen failed: %s", __func__, strerror(errno));

        return false;
    }

    int filesize;

    // png is our only option just using raylib
    unsigned char* png_bytes =
        ExportImageToMemory(im->rayim, ".png", &filesize);

    if (!png_bytes) {

        L_E("%s: Could not write to png: %s", __func__, strerror(errno));

        return false;
    }

    L_I("%s: Wrote png is %0.2fmb", __func__, BYTES_TO_MB(filesize));

    fwrite(png_bytes, 1, filesize, fp);

    pclose(fp);

    // make sure we free it properly
    RL_FREE(png_bytes);

    return true;
}

int doko_paste_image_from_clipboard(doko_control_t* ctrl) {

    return doko_paste_image_from_clipboard_x11(ctrl);
}

bool doko_copy_image_to_clipboard(doko_image_t* im) {

    return doko_copy_image_to_clipboard_x11(im);
}

#else

bool doko_copy_image_to_clipboard(doko_image_t* im) {

    return false;
}

#endif
