
#include <p2g/log.h>
#include <p2g/core.h>

#ifndef LOG_LEVEL_DEFAULT
#define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#endif 

int log_output_level = LOG_LEVEL_DEFAULT;

void p2g_set_fatal_handler(fatal_handler_t fn) {
  p2g_fatal_handler = fn;
}

void p2g_fatal(const char *msg, ...) {
  logerr("FATAL: %s", msg);
  p2g_fatal_handler();
}

void p2g_fatal_busyloop() {
  while(1) {}
}

fatal_handler_t p2g_fatal_handler = p2g_fatal_busyloop;
