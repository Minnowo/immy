
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../external/strnatcmp.h"
#include "doko.h"



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

char* doko_strdupn(const char* str, size_t n, size_t* len_) {

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

char* doko_strdup(const char* str) {

#ifdef _POSIX_C_SOURCE

    return strdup(str);
#else

    return doko_strdupn(str, 0, NULL);

#endif
}

char* doko_estrdup(const char* str) {

    char* newstr = doko_strdup(str);

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

void dokoSetCacheDir(doko_config_t* conf, const char* path) {

    if (path == NULL) {

        if (conf->cacheDir != NULL)
            free(conf->cacheDir);

        conf->cacheDir = dokoGetCacheDirectory();

    } else {

        conf->cacheDir = doko_estrdup(path);
    }
}

char* dokoGetCacheDirectory() {

    char* cache = getenv("XDG_CACHE_HOME");

    if (*cache == 0)
        cache = getenv("HOME");
    ;

    cache = dokoStrJoin(cache, ".cache/doko/", "/");

    DIE_IF_NULL(cache, "%s: failed to join str: %s", strerror(errno));

    return cache;
}



