#ifndef _XLOG_H
#define _XLOG_H

#include "printf.h"

#define LOG_INFO(...) printf("[X-Hyper info] " __VA_ARGS__)
#define LOG_WARN(...) printf("[X-Hyper warn] " __VA_ARGS__)

#endif /* _XLOG_H */
