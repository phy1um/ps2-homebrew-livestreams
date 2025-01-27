
#ifndef P2G_CORE_H
#define P2G_CORE_H

typedef void(*fatal_handler_t)(void);
void p2g_fatal(const char *s, ...);

extern fatal_handler_t p2g_fatal_handler;
void p2g_set_fatal_handler(fatal_handler_t fn);

typedef int(*p2g_app_main_fn)(int argc, char *argv[]);

struct p2g_app {
  p2g_app_main_fn main; 
};

int p2g_app_init(struct p2g_app *app);
int p2g_app_run(struct p2g_app *app, int argc, char *argv[]);

#endif
