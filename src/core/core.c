
#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../darray.h"
#include "core.h"

LogLevel_t log_level      = LOG_LEVEL;

int iAddImage(ImmyControl_t* ctrl, const char* path_) {

    char* path = iStrDup(path_);

    if (!path) {

        L_E("Cannot duplicate string '%s'! %s", strerror(errno));

        return -1;
    }

    L_D("Adding file [%zu]: %s", ctrl->image_files.size, path, path);

    const char* name = GetFileName(path);

    ImmyImage_t i = {
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

    int newImageIndex = ctrl->image_files.size;

    dImmyImageArrAppend(&ctrl->image_files, i);

    return newImageIndex;
}

void iSetImage(ImmyControl_t* ctrl, size_t index) {

    if (index >= ctrl->image_files.size) {

        L_D("Cannot set the image index to %zu", index);

        return;
    }

    ctrl->selected_image = ctrl->image_files.buffer + index;
    ctrl->selected_index = index;
    ctrl->renderFrames   = RENDER_FRAMES;
}

void iLogRaylib(int msgType, const char* fmt, va_list ap) {

    if (log_level == __LOG_LEVEL_NOTHING)
        return;

    LogLevel_t level = __LOG_LEVEL_DEBUG;

    switch (msgType) {
    case LOG_INFO:
        level = LOG_LEVEL_INFO;
        break;
    case LOG_ERROR:
        level = LOG_LEVEL_ERROR;
        break;
    case LOG_WARNING:
        level = LOG_LEVEL_WARN;
        break;
    case LOG_DEBUG:
        level = LOG_LEVEL_DEBUG;
        break;
    default:
        break;
    }

    FILE* stream = stdout;

    if (level == LOG_LEVEL_ERROR) {
        stream = stderr;
    }

    switch (level) {

    case LOG_LEVEL_NOTHING:
        break;

    case LOG_LEVEL_DEBUG:
        if (log_level <= __LOG_LEVEL_DEBUG) {
            fprintf(stream, "[DEBUG] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_INFO:
        if (log_level <= __LOG_LEVEL_INFO) {
            fprintf(stream, "[INFO] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_WARN:
        if (log_level <= __LOG_LEVEL_WARN) {
            fprintf(stream, "[WARNING] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_ERROR:
        if (log_level <= __LOG_LEVEL_ERROR) {
            fprintf(stream, "[ERROR] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;

    case LOG_LEVEL_CRITICAL:

        if (log_level <= __LOG_LEVEL_CRITICAL) {
            fprintf(stream, "[CRITICAL] ");
            vfprintf(stream, fmt, ap);
            putc('\n', stream);
        }
        break;
    }
}

void iLog(LogLevel_t level, FILE* stream, const char* fmt, ...) {

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
