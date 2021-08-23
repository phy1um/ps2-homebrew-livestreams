#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#if 1
#define logmsg(lvl, msg, ...) printf(lvl " @ (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define logmsg(lvl, msg, ...) ((void)0)
#endif

#define trace(msg, ...) logmsg("[TRCE]", msg, ##__VA_ARGS__)
#define info(msg, ...) logmsg("[INFO]", msg, ##__VA_ARGS__)
#define logerr(msg, ...) logmsg("[ERRO]", msg, ##__VA_ARGS__)


#endif
