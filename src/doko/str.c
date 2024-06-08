
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../external/sha256.h"
#include "../external/strnatcmp.h"
#include "../config.h"
#include "doko.h"



int dokoQsortStrcmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strcmp(pa, pb);
}

int dokoQsortNatstrcmp(const void* a, const void* b) {

    char const* pa = *(char const**)a;
    char const* pb = *(char const**)b;

    return strnatcmp(pa, pb);
}

char* dokoStrdupn(const char* str, size_t n, size_t* len_) {

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

char* dokoStrdup(const char* str) {

#ifdef _POSIX_C_SOURCE

    return strdup(str);
#else

    return dokoStrdupn(str, 0, NULL);

#endif
}

char* dokoEstrdup(const char* str) {

    char* newstr = dokoStrdup(str);

    DIE_IF_NULL(newstr, "%s: strdup was null: %s", __func__, strerror(errno));

    return newstr;
}

char* dokoStrJoin(const char* a, const char* b, const char* sep) {

    size_t len = strlen(a) + strlen(b) + strlen(sep) + 1;

    char* newstr = malloc(len);

    if (!dokoStrJoinInto(newstr, len, a, b, sep))
        return NULL;

    return newstr;
}

bool dokoStrJoinInto(
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

const char* dokoGetCacheDirectory() {

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

char* dokoGetCachedPath(const char* path) {

    char buf[DOKO_PATH_MAX + 1];

    char* ptr = realpath(path, buf);

    // hash the absolute path for a 'unique' 
    // short name for the cache
    // we know DOKO_PATH_MAX can ALWAYS fit sha256 has bytes * 3
    SHA256_CTX sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, (BYTE*)ptr, strlen(ptr));
    sha256_final(&sha256, (BYTE*)buf);

    // 0 - 31  contains the sha256 hash bytes
    // 32 - 96 contains the sha256 hex string
    ptr += 32;

    for (size_t i = 0; i < 32; ++i) {
        sprintf(ptr + i * 2, "%02x", (unsigned char)buf[i]);
    }

    ptr[64] = 0;

    const char* cacheDir = dokoGetCacheDirectory();

    char* cachedPath = dokoStrJoin( cacheDir, ptr, THUMBNAIL_CACHE_PATH);

    return cachedPath;
}


bool dokoCreateDirectory(const char* path_){

    char* path = dokoEstrdup(path_);
    char* s    = path;
    bool  r    = true;

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

        if (mkdir(path, 0755) == -1) {

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
