#include <stddef.h>
#include "Utilities.h"
#include "Product.h"

Product *createInstance(char tag)
{
    Product *temp = NULL;
    if (tag == 'N' || tag == 'n') {
        temp = Product_create('N');
    } else if (tag == 'P' || tag == 'p') {
        temp = Product_create('P');
    }
    return temp;
}

// getdelim and getline for the Windows losers
#if defined(USE_BUILTIN_GETLINE) || !(defined(__STDC_LIB_EXT2__) || _POSIX_SOURCE >= 200800 || defined(__GLIBC__) || defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__))
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifndef MALLOC_STEP
#define MALLOC_STEP 128
#endif
ssize_t getdelim(char **buffer, size_t *size, int delim, FILE *stream)
{
    if (size == NULL || buffer == NULL || stream == NULL || feof(stream) || ferror(stream)) {
        errno = EINVAL;
        return -1;
    }
    ssize_t count = 0;
#define RESIZE_IF_NEEDED()                                     \
    do {                                                       \
        if (*size < SSIZE_MAX && (ssize_t)*size - count < 1) { \
            void *ptr = realloc(*buffer, *size + MALLOC_STEP); \
            if (ptr == NULL) {                                 \
                errno = ENOMEM;                                \
                return -1;                                     \
            }                                                  \
            *size += MALLOC_STEP;                              \
            *buffer = (char *)ptr;                             \
        }                                                      \
    } while (0)

    RESIZE_IF_NEEDED();
    for (;;) {
        int c = getc(stream);
        if (c == EOF) {
            return -1;
        }
        (*buffer)[count++] = c;
        RESIZE_IF_NEEDED();
        if (c == delim) {
            break;
        }
    }
    (*buffer)[count] = '\0';
    return count;
}
ssize_t getline(char **buffer, size_t *size, FILE *stream)
{
    return getdelim(buffer, size, '\n', stream);
}

#endif
