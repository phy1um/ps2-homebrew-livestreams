
#ifndef P2G_CORE_H
#define P2G_CORE_H

typedef void(*fatal_handler_t)(void);
void p2g_fatal(const char *s, ...);

extern fatal_handler_t p2g_fatal_handler;
void p2g_set_fatal_handler(fatal_handler_t fn);


#endif
