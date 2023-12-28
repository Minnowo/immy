

#ifndef DOKO_DARRAY_H
#define DOKO_DARRAY_H

#include <stdlib.h>


#define BINARY_SEARCH_INSERT_INDEX(arr, size, target, index)                   \
    do {                                                                       \
        size_t low = 0, high = (size)-1, mid = 0;                              \
        while (low <= high) {                                                  \
            mid = low + (high - low) / 2;                                      \
            if ((arr)[mid] == (target)) {                                      \
                break;                                                         \
            }                                                                  \
            if ((arr)[mid] < (target)) {                                       \
                low = mid + 1;                                                 \
            } else {                                                           \
                high = mid - 1;                                                \
            }                                                                  \
        }                                                                      \
        (index) = mid;                                                         \
    } while (0)


#define DARRAY_DEF(name, type)                                                \
    typedef struct name {                                                      \
            type buffer;                                                       \
            size_t length;                                                     \
            size_t size;                                                       \
    } name##_t;


#define DARRAY_FREE(darray)                                                    \
    do {                                                                       \
        if ((darray).buffer != NULL) {                                         \
            free((darray).buffer);                                             \
            (darray).buffer = NULL;                                            \
        }                                                                      \
    } while (0)


#define DARRAY_INIT(darray, size_initial)                                      \
    do {                                                                       \
        (darray).size = 0;                                                     \
        (darray).length = (size_initial);                                      \
        (darray).buffer = malloc(sizeof((darray).buffer[0]) * (size_initial)); \
        if ((darray).buffer == NULL) {                                         \
            exit(1);                                                           \
        }                                                                      \
    } while (0);


#define DARRAY_APPEND(darray, item)                                            \
    do {                                                                       \
        if ((darray).size >= (darray).length) {                                \
            if ((darray).length == 0) (darray).length = 32;                    \
            (darray).length *= 2;                                              \
            (darray).buffer =                                                  \
                realloc((darray).buffer,                                       \
                        sizeof((darray).buffer[0]) * (darray).length);         \
                                                                               \
            if ((darray).buffer == NULL) {                                     \
                exit(1);                                                       \
            }                                                                  \
        }                                                                      \
        (darray).buffer[(darray).size++] = (item);                             \
    } while (0)



#define DARRAY_FOR_EACH_I(darray, iter)                                        \
    for (size_t iter = 0; iter < (darray).size; ++iter)

#endif