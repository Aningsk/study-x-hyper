#ifndef _UTILS_H
#define _UTILS_H

#include <stddef.h>
#include "types.h"

u64 strlen(const char *s);
void *memset(void *dst, int c, u64 n);
void *memcpy(void *dst, const void *src, size_t count);
char *strcpy(char *dst, const char *src);

#endif
