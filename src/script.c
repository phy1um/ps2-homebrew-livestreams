#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "log.h"
#include "script.h"

struct lua_State *script_load(const char *file) {
  info("init lua with entrypoint %s", file);
  struct lua_State *L;
  L = luaL_newstate();
  if (!L) {
    logerr("failed to start lua state");
    return 0;
  }
  luaL_openlibs(L);
  lua_drawstate_init(L);

  int rc = luaL_loadfile(L, file);
  if (rc) {
    logerr("failed to execute lua file %s", file);
    return 0;
  }
  rc = lua_pcall(L, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(L, -1);
    logerr("lua execution error -- %s", err);
    return 0;
  }
  return L;
}

int script_simple_call(struct lua_State *L, const char *fn) {
  lua_getglobal(L, fn);
  if (lua_pcall(L, 0, 0, 0) != 0) {
    const char *err = lua_tostring(L, -1);
    logerr("lua error calling %s -- %s", fn, err);
    return 1;
  }
  return 0;
}

int script_end(struct lua_State *L) {
  lua_close(L);
  return 0;
}

void script_define_constants(struct lua_State *L) {
  lua_pushinteger(L, 0);
  lua_setglobal(L, "GS_NONINTERLACED");
  lua_pushinteger(L, 1);
  lua_setglobal(L, "GS_INTERLACED");
}
