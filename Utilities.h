#ifndef AMA_UTILITIES_H
#define AMA_UTILITIES_H
#include "Product.h"
#define __STDC_WANT_LIB_EXT2__ 1
#include <stdio.h>
#include <stddef.h>
#include <limits.h>
Product *createInstance(char tag);
#if defined(USE_BUILTIN_GETLINE) || !(defined(__STDC_LIB_EXT2__) || _POSIX_C_SOURCE >= 200800 || defined(__GLIBC__) || defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__))
#if !defined(SSIZE_MAX)
#if defined(__LLP64__) || defined(_WIN64)
typedef long long ssize_t;
#define SSIZE_MAX LLONG_MAX
#define SSIZE_MIN LLONG_MIN
#else
typedef long ssize_t;
#define SSIZE_MAX LONG_MAX
#define SSIZE_MIN LONG_MIN
#endif
#endif

ssize_t getdelim(char **buffer, size_t *size, int delim, FILE *stream);
ssize_t getline(char **buffer, size_t *size, FILE *stream);
#endif

#endif
