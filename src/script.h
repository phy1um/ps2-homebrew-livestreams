
#ifndef SRC_LUA_H
#define SRC_LUA_H

#include <lua.h>

#define LUA_DEF(fn) \
  do { \
    lua_pushcfunction(L, lua_ ## fn); \
    lua_setglobal(L, #fn); } while(0)

struct lua_State * script_load(const char *file);
int script_simple_call(struct lua_State *L, const char *fn);
int script_end(struct lua_State *L);

int lua_drawstate_init(lua_State *L);

#endif
