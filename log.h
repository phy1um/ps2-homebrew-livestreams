#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#if 1
#define info(msg, ...) printf("[INFO] " msg "\n", ##__VA_ARGS__)
#else
#define info(msg, ...) ((void)0)
#endif

#endif
