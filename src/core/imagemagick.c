

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
#include "core.h"


#ifdef __unix__


bool immy_load_with_magick_stdout(const char* path, Image* im) {

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
        char*  new_path = immyStrdupn(path, 3, &n);

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

    }

    L_I("%s: Read %fmb from stdout", __func__, BYTES_TO_MB(data.size));

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

#else


bool immy_load_with_magick_stdout(const char* path, Image* im) {
    return 0;
}

#endif
