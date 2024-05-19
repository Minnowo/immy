
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define genf(out, ...)                                                         \
    do {                                                                       \
        fprintf((out), __VA_ARGS__);                                           \
        fprintf((out), "\n");                                                  \
    } while (0)

#define genfp(out, ...)                                                        \
    do {                                                                       \
        fprintf((out), __VA_ARGS__);                                           \
    } while (0)

typedef struct {
    const char* file_path;
    size_t offset;
    size_t size;
} Resource;

Resource resources[] = {
    {.file_path = "../resources/fonts/unifont-15.1.04.otf"},
};

#define RESOURCES_COUNT (sizeof(resources) / sizeof(resources[0]))

size_t fsize(FILE* f) {

    fseek(f, 0, SEEK_END);

    size_t size = ftell(f);

    fseek(f, 0, SEEK_SET);

    return size;
}

void generate_resource_bundle(void) {
    const char* BUNDLE_H_PATH = "./bundle.h";

    FILE* out = fopen(BUNDLE_H_PATH, "wb");

    FILE* f = NULL;

    size_t total_byte_size = 0;

    if (!out) {

        fprintf(stderr, "Could not read %s: %s", BUNDLE_H_PATH,
                strerror(errno));

        exit(1);
    }

    // generate basic header
    genf(out, "// This code is auto generated!");
    genf(out, "// Any changes will be lost");
    genf(out, "//");
    genf(out, "// clang-format off");
    genf(out, "#ifndef BUNDLE_H_");
    genf(out, "#define BUNDLE_H_");
    genf(out, "typedef struct {");
    genf(out, "    const char *file_path;");
    genf(out, "    size_t offset;");
    genf(out, "    size_t size;");
    genf(out, "} bundle_resource_t;");
    genf(out, "size_t resources_count = %zu;", RESOURCES_COUNT);
    genf(out, "bundle_resource_t resources[] = {");

    // calculate the total bytes for all the resources
    // and also generate the resource definition
    for (size_t i = 0; i < RESOURCES_COUNT; ++i) {

        f = fopen(resources[i].file_path, "r");

        if (!f) {

            fprintf(stderr, "Could not read %s: %s", resources[i].file_path,
                    strerror(errno));

            exit(1);
        }

        resources[i].size = fsize(f);

        resources[i].offset = total_byte_size;

        total_byte_size += resources[i].size;

        genf(out, "    { .file_path = \"%s\", .offset = %zu, .size = %zu },",
             resources[i].file_path, resources[i].offset, resources[i].size);

        fclose(f);
    }

    genf(out, "};");
    genf(out, "unsigned char resource_bundle[] = {");

    size_t read_bytes = 0;
#define LINE_BYTE_COUNT 48

    // generate big array of bytes
    for (size_t i = 0; i < RESOURCES_COUNT; ++i) {

        f = fopen(resources[i].file_path, "r");

        if (!f) {

            fprintf(stderr, "Could not read %s: %s", resources[i].file_path,
                    strerror(errno));

            exit(1);
        }

        char c1;
        size_t read = 1;

        while (read == 1) {
            read = fread(&c1, 1, 1, f);

            read_bytes += read;

            if (read) genfp(out, "0x%02X,", (unsigned char)c1);

            if (read_bytes % LINE_BYTE_COUNT == 0) {

                genfp(out, "\n");
            }
        }

        fclose(f);
    }

    genf(out, "};");
    genf(out, "#endif // BUNDLE_H_");

    fclose(out);
}

int main() {

    generate_resource_bundle();
}
