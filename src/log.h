#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <tamtypes.h>

extern int log_output_level;

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_INFO 5
#define LOG_LEVEL_DEBUG 9
#define LOG_LEVEL_TRACE 15

#ifndef LOG_NO_OUTPUT
#define logmsg(lvl, i, msg, ...)                                               \
  do {                                                                         \
    if (log_output_level >= i) {                                               \
      printf(lvl " (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__);   \
    }                                                                          \
  } while (0)
#else
#define logmsg(lvl, msg, ...) ((void)0)
#endif

#ifdef LOG_TRACE
#define trace(msg, ...) logmsg("[TRCE]", LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define trace(msg, ...) ((void)0)
#endif

#define info(msg, ...) logmsg("[INFO]", LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define logerr(msg, ...) logmsg("[ERRO]", LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define logdbg(msg, ...) logmsg("[DEBG]", LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)

int print_buffer(qword_t *b, int len);

#endif
