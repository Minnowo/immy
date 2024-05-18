
#include <string.h>

#include "resources.h"


#ifdef DOKO_BUNDLE

#include "bundle.h"

#ifndef BUNDLE_H_
#error "DOKO_BUNDLE is defined, but the bundle.h file does not define a bundle"
#endif

unsigned char* get_resource_data(const char* resource_path, size_t *data_size) {

    for (size_t i = 0; i < resources_count; ++i) {

        if (strcmp(resources[i].file_path, resource_path) == 0) {

            *data_size = resources[i].size;

            return &resource_bundle[resources[i].offset];
        }
    }

    return NULL;
}


void free_resource_data(void* data) {

    (void)data;
}


#else


#include "raylib.h"

unsigned char* get_resource_data(const char* resource_path, size_t *data_size) {

    int dataSize;

    void *data = LoadFileData(resource_path, &dataSize);

    if(!data) {
        return NULL;
    }

    *data_size = dataSize;

    return data;
}


void free_resource_data(void* data) {

    UnloadFileData(data);
}


#endif

