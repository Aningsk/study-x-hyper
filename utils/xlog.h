#ifndef _XLOG_H
#define _XLOG_H

#include "printf.h"

#define LOG_INFO(...) printf("[X-Hyper info] " __VA_ARGS__)

#define LOG_NOTICE(...)                                                        \
	do {                                                                   \
		printf("[X-Hyper notice] \e[32m");                             \
		printf(__VA_ARGS__);                                           \
		printf("\e[0m");                                               \
	} while (0)

#define LOG_WARN(...)                                                          \
	do {                                                                   \
		printf("[X-Hyper warn] \e[33m");                               \
		printf(__VA_ARGS__);                                           \
		printf("\e[0m");                                               \
	} while (0)

#define LOG_ERROR(...)                                                         \
	do {                                                                   \
		printf("[X-Hyper error] \e[31m");                              \
		printf(__VA_ARGS__);                                           \
		printf("\e[0m");                                               \
	} while (0)

#endif /* _XLOG_H */
