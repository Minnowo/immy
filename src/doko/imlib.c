
#include <raylib.h>
#include <unistd.h>

#include "doko.h"

bool doko_load_with_imlib2(const char* path, Image* im) {

#if !defined(__unix__) || !defined(IMLIB2_ENABLED)

    return 0;

#else

#include <Imlib2.h>

    L_I("About to read image using Imlib2");

    Imlib_Image image;

    image = imlib_load_image_immediately_without_cache(path);

    if (!image) {

        L_E("Error loading with imlib2: %s", imlib_strerror(imlib_get_error()));

        return false;
    }

    imlib_context_set_image(image);

    im->width   = imlib_image_get_width();
    im->height  = imlib_image_get_height();
    im->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    im->mipmaps = 1;

    size_t pixels = im->width * im->height;

    uint32_t* argb = imlib_image_get_data();

    if (!argb) {

        return false;
    }

    // we have to copy the buffer otherwise we can't free from imlib
    im->data = malloc(pixels * sizeof(Color));

    if (!im->data) {

        L_E("Could not make pixel data: %s", strerror(errno));

        return false;
    }

    Color* d = (Color*)im->data;

    for (size_t i = 0; i < pixels; i++) {

        d[i] = (Color){
            .a = (*argb) >> 24,
            .r = (*argb) >> 16,
            .g = (*argb) >> 8,
            .b = (*argb) >> 0,
        };
        argb++;
    }

    imlib_free_image();

    L_I("Loaded with imlib2");

    return true;

#endif
}
