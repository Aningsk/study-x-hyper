#include <stddef.h>
#include "types.h"

u64 strlen(const char *s)
{
	u64 len = 0;
	while (*s++ != '\0')
		len++;
	return len;
}

void *memset(void *dst, int c, u64 n)
{
	char *d = dst;
	while (n-- > 0) {
		*d++ = c;
	}
	return dst;
}

void *memcpy(void *dst, const void *src, size_t count)
{
	char *to = (char *)dst;
	char *from = (char *)src;
	while (count-- > 0) {
		*to++ = *from++;
	}
	return dst;
}

char *strcpy(char *dst, const char *src)
{
	char *r = dst;
	while ((*dst++ = *src++) != 0)
		;
	return r;
}
