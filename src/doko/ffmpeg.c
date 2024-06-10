

#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __unix__
#include <sys/wait.h>
#endif

#include "../config.h"
#include "../darray.h"
#include "../external/strnatcmp.h"
#include "doko.h"


#ifdef __unix__


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


#else 



bool doko_load_with_ffmpeg_stdout(const char* path, Image* im) {
    return 0;
}

#endif
