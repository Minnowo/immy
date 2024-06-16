
#ifndef IMMY_DARRAY_H
#define IMMY_DARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BINARY_SEARCH_INSERT_INDEX(arr, size, target, index)                                                           \
    do {                                                                                                               \
        size_t low = 0, high = (size_t)(size) - 1, mid = 0;                                                            \
        while (low <= high) {                                                                                          \
            mid = low + (high - low) / 2;                                                                              \
            if ((arr)[mid] == (target)) {                                                                              \
                break;                                                                                                 \
            }                                                                                                          \
            if ((arr)[mid] < (target)) {                                                                               \
                low = mid + 1;                                                                                         \
            } else {                                                                                                   \
                if (mid == 0)                                                                                          \
                    break;                                                                                             \
                high = mid - 1;                                                                                        \
            }                                                                                                          \
        }                                                                                                              \
        (index) = mid;                                                                                                 \
    } while (0)

typedef struct DArray {
        void*  buffer;
        size_t length;
        size_t size;
} DArray_t;

#define DARRAY_DEF(name, type)                                                                                         \
    typedef struct name {                                                                                              \
            type*  buffer;                                                                                             \
            size_t length;                                                                                             \
            size_t size;                                                                                               \
    } name##_t;                                                                                                        \
    static inline void name##Free(name##_t* darray) {                                                                  \
        dArrayFree((DArray_t*)darray);                                                                                 \
    }                                                                                                                  \
    static inline bool name##Init(name##_t* darray, size_t size_initial) {                                             \
        return dArrayInit((DArray_t*)darray, sizeof(type), size_initial);                                              \
    }                                                                                                                  \
    static inline bool name##GrowSize(name##_t* darray, size_t size_) {                                                \
        return dArrayGrowSize((DArray_t*)darray, sizeof(type), size_);                                                 \
    }                                                                                                                  \
    static inline bool name##Append(name##_t* darray, type item) {                                                     \
        return dArrayAppend((DArray_t*)darray, sizeof(item), &item);                                                  \
    }

#define DARRAY_FOR_EACH(darray, iter) for (size_t(iter) = 0; (iter) < (darray).size; ++(iter))

#define DARRAY_FOR_EACH_I(darray, iter) for (; (iter) < (darray).size; ++(iter))

#define DARRAY_FOR_I(darray, iter, until) for (; (iter) < (darray).size && (iter) < (until); ++(iter))


void dArrayFree(DArray_t* darray);
bool dArrayInit(DArray_t* darray, size_t item_size, size_t size_initial);
bool dArrayGrowSize(DArray_t* darray, size_t item_size, size_t size_);
bool dArrayAppend(DArray_t* darray, size_t item_size, void* item);

#ifdef DARRAY_IMPLEMENTATION

void dArrayFree(DArray_t* darray) {

    free(darray->buffer);

    darray->buffer = NULL;
    darray->size   = 0;
    darray->length = 0;
}

bool dArrayInit(DArray_t* darray, size_t item_size, size_t size_initial) {

    darray->size   = 0;
    darray->length = size_initial;
    darray->buffer = calloc(item_size, size_initial);

    return darray->buffer == NULL;
}

bool dArrayGrowSize(DArray_t* darray, size_t item_size, size_t size_) {

    if (size_ > darray->length) {

        if (darray->length == 0)
            darray->length = 32;

        while (size_ > darray->length)
            darray->length *= 2;

        darray->buffer = realloc(darray->buffer, item_size * darray->length);

        if (darray->buffer == NULL)
            return false;

        /* you have to zero out when using realloc */
        memset((char*)darray->buffer + darray->size, 0, item_size * (darray->length - darray->size));

        darray->size = size_;
    }

    return true;
}

bool dArrayAppend(DArray_t* darray, size_t item_size, void* item) {

    if (darray->size >= darray->length) {

        if (darray->length == 0)
            darray->length = 32;

        darray->length *= 2;

        darray->buffer = realloc(darray->buffer, item_size * darray->length);

        if (darray->buffer == NULL) {
            return false;
        } 

        /* you have to zero out when using realloc */
        memset((char*)darray->buffer + darray->size, 0, item_size * (darray->length - darray->size));
    }
    
    memcpy((char*)darray->buffer + item_size * darray->size, item, item_size);

    darray->size++;

    return true;
}

#endif

#endif
