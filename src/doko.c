
// needed for popen
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __unix__
#include <sys/wait.h>
#endif

#include "config.h"
#include "darray.h"
#include "doko.h"
#include "external/strnatcmp.h"

int info_bar_height = INFO_BAR_HEIGHT;
int log_level       = LOG_LEVEL;

#ifdef __unix__

#define PIPE_READ 0
#define PIPE_WRITE 1

bool doko_copy_image_to_clipboard_x11(doko_image_t* im) {

    L_I("%s: Copying image: %d x %d", __func__, im->rayim.width,
        im->rayim.height);

    FILE* fp = popen(X11_COPY_IMAGE_COMMAND, "w");

    if (!fp) {

        L_E("%s: popen failed: %s", __func__, strerror(errno));

        return false;
    }

    int filesize;

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

bool doko_copy_image_to_clipboard(doko_image_t* im) {

    return doko_copy_image_to_clipboard_x11(im);
}

bool doko_load_with_ffmpeg_stdout(const char* path, Image* im) {

    L_I("About to read image using FFMPEG");
    L_D("%s: decode format is " FFMPEG_CONVERT_MIDDLE_FMT, __func__);

    int pipefd[2];

    if (pipe(pipefd) < 0) {

        L_W("%s: Could not create a pipe: %s", __func__, strerror(errno));

        return false;
    }

    pid_t child = fork();

    if (child < 0) {

        L_W("%s: Could not create fork: %s", __func__, strerror(errno));

        return false;
    }

    if (child == 0) {
        // in child process

        if (dup2(pipefd[PIPE_WRITE], STDOUT_FILENO) < 0) {

            L_C("%s (child): Could not open write end of pipe "
                "as stdout: %s",
                __func__, strerror(errno));

            exit(EXIT_FAILURE);

            return false;
        }

        close(pipefd[PIPE_READ]);

        // clang-format off
        int ret = execlp(
            "ffmpeg", 
            "ffmpeg", 
            "-v", FFMPEG_VERBOSITY, 
            "-nostdin",
            "-hide_banner", 
            "-i", path, 
            "-c:v", FFMPEG_CONVERT_MIDDLE_FMT, 
            "-f", "image2pipe", 
            "-", 
            NULL
        );
        // clang-format on

        if (ret < 0) {

            L_C("%s (child): execlp failed: %s", __func__, strerror(errno));

            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

        return false;
    }

    if (close(pipefd[PIPE_WRITE]) < 0) {

        L_W("%s: Could not close write stream: %s", __func__, strerror(errno));
    }

    ssize_t bytesRead;

    dbyte_arr_t data;

    DARRAY_INIT(data, 1024 * 1024 * 4);

    while ((bytesRead = read(
                pipefd[PIPE_READ], data.buffer + data.size,
                data.length - data.size
            )) > 0) {

        data.size += bytesRead;

        if (data.size == data.length) {
            DARRAY_APPEND(data, 0);
            data.size--;
        }

        L_D("%s: Read %fmb from stdout", __func__, BYTES_TO_MB(data.size));
    }
    L_I("%s: Read %fmb from stdout", __func__, BYTES_TO_MB(data.size));

    wait(NULL);
    close(pipefd[PIPE_READ]);

    if (data.size > 0) {

        *im = LoadImageFromMemory(
            "." FFMPEG_CONVERT_MIDDLE_FMT, data.buffer, data.size
        );
    }

    DARRAY_FREE(data);

    return im->data != NULL;
}

bool doko_load_with_magick_stdout(const char* path, Image* im) {

    L_I("About to read image using ImageMagick Convert");
    L_D("%s: Convert decode format is " MAGICK_CONVERT_MIDDLE_FMT, __func__);

    int pipefd[2];

    if (pipe(pipefd) < 0) {

        L_W("%s: Could not create a pipe: %s", __func__, strerror(errno));

        return false;
    }

    pid_t child = fork();

    if (child < 0) {

        L_W("%s: Could not create fork: %s", __func__, strerror(errno));

        return false;
    }

    if (child == 0) {
        // in child process

        if (dup2(pipefd[PIPE_WRITE], STDOUT_FILENO) < 0) {

            L_C("%s (child): Could not open "
                "write end of pipe as stdout: %s",
                __func__, strerror(errno));

            exit(EXIT_FAILURE);

            return false;
        }

        close(pipefd[PIPE_READ]);

        size_t n;
        char*  new_path = doko_strdupn(path, 3, &n);

        if (new_path == NULL) {

            L_C("%s (child): Could not dup str: %s", __func__, strerror(errno));

            exit(EXIT_FAILURE);

            return false;
        }

        // to tell imagemagick we want the first image only
        // we have to append [0] to the end of the path
        new_path[n - 1] = ']';
        new_path[n - 2] = '0';
        new_path[n - 3] = '[';

        // clang-format off
        int ret = execlp(
            "convert", 
            "convert", 
            "-quiet", 
            new_path,
            MAGICK_CONVERT_MIDDLE_FMT ":-", 
            NULL
        );
        // clang-format on

        free(new_path);

        if (ret < 0) {

            L_C("%s: execlp failed: %s", __func__, strerror(errno));

            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);

        return false;
    }

    if (close(pipefd[PIPE_WRITE]) < 0) {

        L_W("%s: Could not close write stream: %s", __func__, strerror(errno));
    }

    ssize_t bytesRead;

    dbyte_arr_t data;

    DARRAY_INIT(data, 1024 * 1024 * 4);

    while ((bytesRead = read(
                pipefd[PIPE_READ], data.buffer + data.size,
                data.length - data.size
            )) > 0) {

        data.size += bytesRead;

        if (data.size == data.length) {
            DARRAY_APPEND(data, 0);
            data.size--;
        }

        L_D("%s: Read %fmb from stdout", __func__, BYTES_TO_MB(data.size));
    }

    L_I("%s: Read %fmb from stdout", BYTES_TO_MB(data.size));

    wait(NULL);
    close(pipefd[PIPE_READ]);

    if (data.size > 0) {

        *im = LoadImageFromMemory(
            "." MAGICK_CONVERT_MIDDLE_FMT, data.buffer, data.size
        );
    }

    DARRAY_FREE(data);

    return im->data != NULL;
}


bool doko_load_with_imlib2(const char* path, Image* im) {

#if (USE_IMLIB2 == 1)

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

#else

    return 0;

#endif
}

#else

bool doko_load_with_magick_stdout(const char* path, Image* im) {
    return 0;
}

bool doko_load_with_ffmpeg_stdout(const char* path, Image* im) {
    return 0;
}

bool doko_load_with_imlib2(const char* path, Image* im) {
    return 0;
}

bool doko_copy_image_to_clipboard(doko_image_t* im) {

    return false;
}

#endif

bool doko_loadImage(doko_image_t* image) {

    if (image->status == IMAGE_STATUS_LOADED) {
        return 1;
    }

    if (!(USE_IMLIB2 && doko_load_with_imlib2(image->path, &image->rayim)) &&
        !(USE_MAGICK && doko_load_with_magick_stdout(image->path, &image->rayim)
        ) &&
        !(USE_FFMPEG && doko_load_with_ffmpeg_stdout(image->path, &image->rayim)
        )) {

        image->rayim = LoadImage(image->path);
    }

    if (!IsImageReady(image->rayim)) {

        image->status = IMAGE_STATUS_FAILED;

        L_W("Could not load image %s", image->path);

        return 0;
    }

    image->srcRect = (Rectangle){
        0.0,
        0.0,
        image->rayim.width,
        image->rayim.height,
    };
    image->dstPos = (Vector2){0, 0};
    image->status = IMAGE_STATUS_LOADED;

    return 1;
}

void doko_fitCenterImage(doko_image_t* image) {

    int sw = ImageViewWidth;
    int sh = ImageViewHeight;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->scale = fmin((double)sw / iw, (double)sh / ih);

    image->dstPos.x = (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = (sh / 2.0) - (ih * image->scale) / 2.0;
}

void doko_centerImage(doko_image_t* image) {

    int sw = ImageViewWidth;
    int sh = ImageViewHeight;
    int iw = image->srcRect.width;
    int ih = image->srcRect.height;

    image->dstPos.x = (sw / 2.0) - (iw * image->scale) / 2.0;
    image->dstPos.y = (sh / 2.0) - (ih * image->scale) / 2.0;
}

void doko_ensureImageNotLost(doko_image_t* image) {

    int   sw = ImageViewWidth - IMAGE_INVERSE_MARGIN_X;
    int   sh = ImageViewHeight - IMAGE_INVERSE_MARGIN_X;
    float iw = IMAGE_INVERSE_MARGIN_X - (image->srcRect.width * image->scale);
    float ih = IMAGE_INVERSE_MARGIN_Y -(image->srcRect.height * image->scale);

    if (image->dstPos.x > sw) {

        image->dstPos.x = sw;

    } else if (image->dstPos.x < iw) {

        image->dstPos.x = iw;
    }

    if (image->dstPos.y > sh) {

        image->dstPos.y = sh;

    } else if (image->dstPos.y < ih) {

        image->dstPos.y = ih;
    }
}

void doko_moveScrFracImage(doko_image_t* im, double xFrac, double yFrac) {

    im->dstPos.x += ImageViewWidth * xFrac;
    im->dstPos.y += ImageViewHeight * yFrac;
    doko_ensureImageNotLost(im);
}

void doko_zoomImageCenter(doko_image_t* im, double afterZoom) {

    double beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    //clang-format off
    im->dstPos.x -= (int)((im->srcRect.width * afterZoom) -
                          (im->srcRect.width * beforeZoom)) >> 1;

    im->dstPos.y -= (int)((im->srcRect.height * afterZoom) -
                          (im->srcRect.height * beforeZoom)) >> 1;
    //clang-format on

    im->scale = afterZoom;

    doko_ensureImageNotLost(im);
}

void doko_zoomImageOnPoint(doko_image_t* im, double afterZoom, int x, int y) {

    double beforeZoom = im->scale;

    if (afterZoom < 0) {
        afterZoom = SMALLEST_SCALE_VALUE;
    }

    double scaleRatio =
        (im->srcRect.width * beforeZoom) / (im->srcRect.width * afterZoom);

    double mouseOffsetX = x - im->dstPos.x;
    double mouseOffsetY = y - im->dstPos.y;

    im->dstPos.x -= (int)((mouseOffsetX / scaleRatio) - mouseOffsetX);
    im->dstPos.y -= (int)((mouseOffsetY / scaleRatio) - mouseOffsetY);
    im->scale = afterZoom;

    doko_ensureImageNotLost(im);
}

void doko_zoomImageCenterFromClosest(doko_image_t* im, bool zoomIn) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {

        while (index != ZOOM_LEVELS_SIZE && ZOOM_LEVELS[index] <= im->scale) {
            index++;
        }

    } else {

        while (index != 0 && ZOOM_LEVELS[index] >= im->scale) {
            index--;
        }
    }

    doko_zoomImageCenter(im, ZOOM_LEVELS[index]);
}

void doko_zoomImageOnPointFromClosest(
    doko_image_t* im, bool zoomIn, int x, int y
) {

    size_t index;
    BINARY_SEARCH_INSERT_INDEX(ZOOM_LEVELS, ZOOM_LEVELS_SIZE, im->scale, index);

    if (zoomIn) {

        while (index != ZOOM_LEVELS_SIZE && ZOOM_LEVELS[index] <= im->scale) {
            index++;
        }

    } else {

        while (index != 0 && ZOOM_LEVELS[index] >= im->scale) {
            index--;
        }
    }

    doko_zoomImageOnPoint(im, ZOOM_LEVELS[index], x, y);
}

char* doko_strdupn(const char* str, size_t n, size_t* len_) {

    const size_t len = strlen(str);

    char* newstr = malloc(len + n + 1);

    if (!newstr) {
        return NULL;
    }

    memcpy(newstr, str, len);
    memset(newstr + len, 0, n + 1);

    if (len_ != NULL) {
        *len_ = len + n;
    }

    return newstr;
}

char* doko_strdup(const char* str) {

#ifdef _POSIX_C_SOURCE

    return strdup(str);

#else

    return doko_strdupn(str, 0, NULL);

#endif
}

int doko_qsort_strcmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strcmp(pa, pb);
}

int doko_qsort_natstrcmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strnatcmp(pa, pb);
}

void set_image(doko_control_t* ctrl, size_t index) {

    if (index >= ctrl->image_files.size) {

        L_D("Cannot set the image index to %zu", index);

        return;
    }

    ctrl->selected_image = ctrl->image_files.buffer + index;
    ctrl->selected_index = index;
    ctrl->renderFrames   = RENDER_FRAMES;
}

void doko_log(log_level_t level, FILE* stream, const char* fmt, ...) {

    if (log_level == __LOG_LEVEL_NOTHING)
        return;

    va_list ap;

    switch (level) {

    case LOG_LEVEL_NOTHING:
        break;

    case LOG_LEVEL_DEBUG:
        if (log_level <= __LOG_LEVEL_DEBUG) {
            va_start(ap, fmt);
            fprintf(stream, "[DEBUG] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_INFO:
        if (log_level <= __LOG_LEVEL_INFO) {
            va_start(ap, fmt);
            fprintf(stream, "[INFO] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_WARN:
        if (log_level <= __LOG_LEVEL_WARN) {
            va_start(ap, fmt);
            fprintf(stream, "[WARNING] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_ERROR:
        if (log_level <= __LOG_LEVEL_ERROR) {
            va_start(ap, fmt);
            fprintf(stream, "[ERROR] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;

    case LOG_LEVEL_CRITICAL:

        if (log_level <= __LOG_LEVEL_CRITICAL) {
            va_start(ap, fmt);
            fprintf(stream, "[CRITICAL] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
            va_end(ap);
        }
        break;
    }
}


// edited from the raylib ImageResizeNN function
bool doko_copy_and_resize_image_nn(
    const Image* image, Image* newimage, int newWidth, int newHeight
) {
    // Security check to avoid program crash
    if ((image->data == NULL) || (image->width == 0) || (image->height == 0))
        return false;

    Color* pixels;

    if (image->format != PIXELFORMAT_UNCOMPRESSED_R32G32B32A32) { 
        pixels = LoadImageColors(*image);
    } else { 
        pixels = image->data;
    }

    Color* output = (Color*)RL_MALLOC(newWidth * newHeight * sizeof(Color));

    // EDIT: added +1 to account for an early rounding problem
    int xRatio = (int)((image->width << 16) / newWidth) + 1;
    int yRatio = (int)((image->height << 16) / newHeight) + 1;

    int x2, y2;

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            x2 = ((x * xRatio) >> 16);
            y2 = ((y * yRatio) >> 16);

            output[(y * newWidth) + x] = pixels[(y2 * image->width) + x2];
        }
    }

    int format = image->format;

    newimage->data    = output;
    newimage->width   = newWidth;
    newimage->height  = newHeight;
    newimage->mipmaps = 1;
    newimage->format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // Reformat 32bit RGBA image to original format
    ImageFormat(
        newimage, format
    );

    if (image->format != PIXELFORMAT_UNCOMPRESSED_R32G32B32A32) {
        UnloadImageColors(pixels);
    }

    return true;
}

bool doko_create_thumbnail( const Image* image, Image* newimage, int newWidth, int newHeight) {

    double ratio;

    if(image->width > image->height) {

        ratio = (double)image->height / image->width;

        return doko_copy_and_resize_image_nn(image, newimage, newWidth, ratio * newHeight);
    }
    else {
        ratio = (double)image->width / image->height;

        return doko_copy_and_resize_image_nn(image, newimage, ratio * newWidth, newHeight);
    }

    return true;
}

const char* get_pretty_screen_text(doko_screen_t screen) {

    switch (screen) {

    case DOKO_SCREEN_THUMB_GRID:
        return "SCREEN_THUMB";

    case DOKO_SCREEN_IMAGE:
        return "SCREEN_IMAGE";

    case DOKO_SCREEN_FILE_LIST:
        return "SCREEN_FILE_LIST";

    case DOKO_SCREEN_KEYBINDS:
        return "SCREEN_KEYBINDS";

    case DOKO_SCREEN__ALL:
        return "SCREEN_ANY";
    }
}

#define STRINGIFY(x) #x

#define SWITCH_CASE_ENUM_TO_STRING(enum_name)                                  \
    case (enum_name) | SHIFT_MASK:                                             \
        return " S " STRINGIFY(enum_name);                                     \
    case (enum_name) | CONTROL_MASK:                                           \
        return "C  " STRINGIFY(enum_name);                                     \
    case (enum_name) | CONTROL_MASK | SHIFT_MASK:                              \
        return "CS " STRINGIFY(enum_name);                                     \
    case (enum_name):                                                          \
        return "   " STRINGIFY(enum_name);

const char* get_key_to_pretty_text(int key) {

    switch (key) {

        SWITCH_CASE_ENUM_TO_STRING(KEY_NULL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_APOSTROPHE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_COMMA)
        SWITCH_CASE_ENUM_TO_STRING(KEY_MINUS)
        SWITCH_CASE_ENUM_TO_STRING(KEY_PERIOD)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SLASH)
        SWITCH_CASE_ENUM_TO_STRING(KEY_ZERO)
        SWITCH_CASE_ENUM_TO_STRING(KEY_ONE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_TWO)
        SWITCH_CASE_ENUM_TO_STRING(KEY_THREE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_FOUR)
        SWITCH_CASE_ENUM_TO_STRING(KEY_FIVE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SIX)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SEVEN)
        SWITCH_CASE_ENUM_TO_STRING(KEY_EIGHT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_NINE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SEMICOLON)
        SWITCH_CASE_ENUM_TO_STRING(KEY_EQUAL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_A)
        SWITCH_CASE_ENUM_TO_STRING(KEY_B)
        SWITCH_CASE_ENUM_TO_STRING(KEY_C)
        SWITCH_CASE_ENUM_TO_STRING(KEY_D)
        SWITCH_CASE_ENUM_TO_STRING(KEY_E)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F)
        SWITCH_CASE_ENUM_TO_STRING(KEY_G)
        SWITCH_CASE_ENUM_TO_STRING(KEY_H)
        SWITCH_CASE_ENUM_TO_STRING(KEY_I)
        SWITCH_CASE_ENUM_TO_STRING(KEY_J)
        SWITCH_CASE_ENUM_TO_STRING(KEY_K)
        SWITCH_CASE_ENUM_TO_STRING(KEY_L)
        SWITCH_CASE_ENUM_TO_STRING(KEY_M)
        SWITCH_CASE_ENUM_TO_STRING(KEY_N)
        SWITCH_CASE_ENUM_TO_STRING(KEY_O)
        SWITCH_CASE_ENUM_TO_STRING(KEY_P)
        SWITCH_CASE_ENUM_TO_STRING(KEY_Q)
        SWITCH_CASE_ENUM_TO_STRING(KEY_R)
        SWITCH_CASE_ENUM_TO_STRING(KEY_S)
        SWITCH_CASE_ENUM_TO_STRING(KEY_T)
        SWITCH_CASE_ENUM_TO_STRING(KEY_U)
        SWITCH_CASE_ENUM_TO_STRING(KEY_V)
        SWITCH_CASE_ENUM_TO_STRING(KEY_W)
        SWITCH_CASE_ENUM_TO_STRING(KEY_X)
        SWITCH_CASE_ENUM_TO_STRING(KEY_Y)
        SWITCH_CASE_ENUM_TO_STRING(KEY_Z)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT_BRACKET)
        SWITCH_CASE_ENUM_TO_STRING(KEY_BACKSLASH)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT_BRACKET)
        SWITCH_CASE_ENUM_TO_STRING(KEY_GRAVE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SPACE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_ESCAPE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_ENTER)
        SWITCH_CASE_ENUM_TO_STRING(KEY_TAB)
        SWITCH_CASE_ENUM_TO_STRING(KEY_BACKSPACE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_INSERT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_DELETE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_DOWN)
        SWITCH_CASE_ENUM_TO_STRING(KEY_UP)
        SWITCH_CASE_ENUM_TO_STRING(KEY_PAGE_UP)
        SWITCH_CASE_ENUM_TO_STRING(KEY_PAGE_DOWN)
        SWITCH_CASE_ENUM_TO_STRING(KEY_HOME)
        SWITCH_CASE_ENUM_TO_STRING(KEY_END)
        SWITCH_CASE_ENUM_TO_STRING(KEY_CAPS_LOCK)
        SWITCH_CASE_ENUM_TO_STRING(KEY_SCROLL_LOCK)
        SWITCH_CASE_ENUM_TO_STRING(KEY_NUM_LOCK)
        SWITCH_CASE_ENUM_TO_STRING(KEY_PRINT_SCREEN)
        SWITCH_CASE_ENUM_TO_STRING(KEY_PAUSE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F1)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F2)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F3)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F4)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F5)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F6)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F7)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F8)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F9)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F10)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F11)
        SWITCH_CASE_ENUM_TO_STRING(KEY_F12)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT_SHIFT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT_CONTROL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT_ALT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_LEFT_SUPER)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT_SHIFT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT_CONTROL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT_ALT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_RIGHT_SUPER)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KB_MENU)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_0)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_1)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_2)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_3)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_4)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_5)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_6)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_7)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_8)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_9)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_DECIMAL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_DIVIDE)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_MULTIPLY)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_SUBTRACT)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_ADD)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_ENTER)
        SWITCH_CASE_ENUM_TO_STRING(KEY_KP_EQUAL)
        SWITCH_CASE_ENUM_TO_STRING(KEY_BACK)
        // SWITCH_CASE_ENUM_TO_STRING(KEY_MENU)
        SWITCH_CASE_ENUM_TO_STRING(KEY_VOLUME_UP)
        SWITCH_CASE_ENUM_TO_STRING(KEY_VOLUME_DOWN)
    }
    return "UNKNOWN";
}

const char* get_mouse_to_pretty_text(int key) {

    switch (key) {

        SWITCH_CASE_ENUM_TO_STRING(MOUSE_WHEEL_BWD)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_WHEEL_FWD)

        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_LEFT)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_RIGHT)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_MIDDLE)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_SIDE)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_EXTRA)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_FORWARD)
        SWITCH_CASE_ENUM_TO_STRING(MOUSE_BUTTON_BACK)
    }
    return "UNKNOWN";
}


