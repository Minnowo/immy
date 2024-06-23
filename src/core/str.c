
#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#    include <direct.h>
#    define realpath(N, R) _fullpath((R), (N), IMMY_PATH_MAX)
#    define mkdir _mkdir
#else
#    include <sys/stat.h>
#    include <sys/types.h>
#endif

#include "../external/sha256.h"
#include "../external/strnatcmp.h"
#include "../config.h"
#include "core.h"



int iqStrCmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strcmp(pa, pb);
}

int iqNatStrCmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strnatcmp(pa, pb);
}

bool iEndsWith(const char *str, const char *suffix, bool ignorecase) {

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len) {
        return false;
    }

    if (!ignorecase)
        return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;

    return strncasecmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

char* inStrDup(const char* str, size_t n, size_t* len_) {

    const size_t len = strlen(str);

    char* newstr = malloc(len + n + 1);

    if (!newstr)
        return NULL;

    memcpy(newstr, str, len);
    memset(newstr + len, 0, n + 1);

    if (len_ != NULL)
        *len_ = len + n;

    return newstr;
}

char* iStrDup(const char* str) {

#ifdef _POSIX_C_SOURCE

    return strdup(str);
#else

    return inStrDup(str, 0, NULL);

#endif
}

char* ieStrDup(const char* str) {

    char* newstr = iStrDup(str);

    DIE_IF_NULL(newstr, "%s: strdup was null: %s", __func__, strerror(errno));

    return newstr;
}

char* iStrJoin(const char* a, const char* b, const char* sep) {

    size_t len = strlen(a) + strlen(b) + strlen(sep) + 1;

    char* newstr = malloc(len);

    if (!iStrJoinInto(newstr, len, a, b, sep))
        return NULL;

    return newstr;
}

bool iStrJoinInto(
    char* restrict buf, 
    size_t bufSize, 
    const char* a, 
    const char* b,
    const char* sep
) {

    if (!buf)
        return false;

    snprintf(buf, bufSize, "%s%s%s", a, sep, b);

    return true;
}

const char* iGetCacheDirectory() {

#if OVERRIDE_THUMBNAIL_CACHE_PATH

    return THUMBNAIL_BASE_CACHE_PATH;
#else

    char* cache = getenv("XDG_CACHE_HOME");

    if (!cache || *cache == 0)
        cache = getenv("HOME");

    if (!cache || *cache == 0)
        cache = "/tmp";

    return cache;
#endif
}

char* iGetCachedPath(const char* path) {

    char buf[IMMY_PATH_MAX + 1];

    char* ptr = realpath(path, buf);

    // hash the absolute path for a 'unique' 
    // short name for the cache
    // we know IMMY_PATH_MAX can ALWAYS fit SHA256_BLOCK_SIZE * 3
    SHA256_CTX sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, (BYTE*)ptr, strlen(ptr));
    sha256_final(&sha256, (BYTE*)buf);

    // 0 - 31  contains the sha256 hash bytes
    // 32 - 96 contains the sha256 hex string
    ptr += SHA256_BLOCK_SIZE;

    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        sprintf(ptr + i * 2, "%02x", (unsigned char)buf[i]);
    }

    // end the hex string
    ptr[SHA256_BLOCK_SIZE] = 0;

    const char* cacheDir = iGetCacheDirectory();

    char* cachedPath = iStrJoin( cacheDir, ptr, THUMBNAIL_CACHE_PATH);

    return cachedPath;
}


bool iCreateDirectory(const char* path_){

    char* path = iStrDup(path_);
    char* s    = path;
    bool  r    = true;

    if (!path)
        return false;

    while(*s != 0) {

        if (*s == '/')
            s++;

        while(*s != 0 && *s != '/') 
            s++;

        if(*s == 0)
            break;

        char c = *s;

        *s = 0;

        L_D("Creating %s", path);

#ifdef _WIN32
        if (mkdir(path) == -1) {
#else
        if (mkdir(path, 0755) == -1) {
#endif

            if (errno != EEXIST) {

                L_E("%s: could not mkdir: %s", __func__, strerror(errno));

                r = false;

                break;
            }
        }

        *s = c;
    }

    free(path);

    return r;
}
