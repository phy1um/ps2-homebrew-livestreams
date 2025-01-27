
#include <p2g/log.h>
#include <p2g/core.h>
#include "director.h"

#ifndef LOG_LEVEL_DEFAULT
#define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#endif

int p2g_app_init(struct p2g_app *app) {
  if (!app->main) {
    logerr("NULL main function provided");
    return 1;
  }
  return 0;
}

int p2g_app_run(struct p2g_app *app, int argc, char *argv[]) {
  if (director_run()) {
    p2g_fatal("director startup failed");
    return 1;
  }
  int rv = app->main(argc, argv);
  p2g_fatal("main fallthrough: return value=%d", rv);
  return rv;
}

int log_output_level = LOG_LEVEL_DEFAULT;

void p2g_set_fatal_handler(fatal_handler_t fn) {
  p2g_fatal_handler = fn;
}

void p2g_fatal(const char *msg, ...) {
  logerr("FATAL: %s", msg);
  p2g_fatal_handler();
}

void p2g_fatal_busyloop() {
  while (1) {
  }
}

fatal_handler_t p2g_fatal_handler = p2g_fatal_busyloop;
