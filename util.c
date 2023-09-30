

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "doko.h"

void error(int status, int error, const char *fmt, ...)
{
	va_list ap;
    
    fflush(stdout);

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);

	va_end(ap);

	if (error != 0)
		fprintf(stderr, "%s%s", fmt != NULL ? ": " : "", strerror(error));

	fputc('\n', stderr);

    if(status != 0)
        exit(status);
}

void* emalloc(size_t size)
{
	void *ptr;
	
	ptr = malloc(size);

	if (ptr == NULL)
		error(EXIT_FAILURE, errno, NULL);

	return ptr;
}

void* erealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);

	if (ptr == NULL)
		error(EXIT_FAILURE, errno, NULL);

	return ptr;
}

char* estrdup(const char *s)
{
	size_t n = strlen(s) + 1;

	char *d = malloc(n);

	if (d == NULL)
		error(EXIT_FAILURE, errno, NULL);

	memcpy(d, s, n);

	return d;
}