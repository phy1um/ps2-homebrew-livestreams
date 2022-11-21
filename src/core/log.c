#include <lua.h>
#include <stdio.h>

#include <p2g/log.h>
#include <p2g/utils.h>

static inline void lua_log_message(lua_State *l, int level, const char *pre) {
  if (log_output_level >= level) {
    lua_Debug ar;
    lua_getstack(l, 1, &ar);
    lua_getinfo(l, "Sl", &ar);
    int last_sep = last_index_of(ar.short_src, LUA_IDSIZE, '/');
    const char *lua_msg = lua_tostring(l, 1);
    printf("%s (%s:%d) %s\n", pre, (ar.short_src + last_sep + 1),
           ar.currentline, lua_msg);
  }
}

static int lua_log_trace(lua_State *l) {
#ifdef LOG_TRACE
  lua_log_message(l, LOG_LEVEL_TRACE, "[TRCE]");
#endif
  return 0;
}

static int lua_log_debug(lua_State *l) {
  lua_log_message(l, LOG_LEVEL_DEBUG, "[DEBG]");
  return 0;
}
static int lua_log_info(lua_State *l) {
  lua_log_message(l, LOG_LEVEL_INFO, "[INFO]");
  return 0;
}

static int lua_log_warn(lua_State *l) {
  lua_log_message(l, LOG_LEVEL_WARN, "[WARN]");
  return 0;
}

static int lua_log_error(lua_State *l) {
  lua_log_message(l, LOG_LEVEL_ERROR, "[ERRO]");
  return 0;
}

int log_lua_init(lua_State *l) {
  lua_createtable(l, 0, 10);
  lua_pushcfunction(l, lua_log_error);
  lua_setfield(l, -2, "error");
  lua_pushcfunction(l, lua_log_warn);
  lua_setfield(l, -2, "warn");
  lua_pushcfunction(l, lua_log_info);
  lua_setfield(l, -2, "info");
  lua_pushcfunction(l, lua_log_debug);
  lua_setfield(l, -2, "debug");
  lua_pushcfunction(l, lua_log_trace);
  lua_setfield(l, -2, "trace");

  lua_pushinteger(l, LOG_LEVEL_ERROR);
  lua_setfield(l, -2, "errorLevel");
  lua_pushinteger(l, LOG_LEVEL_WARN);
  lua_setfield(l, -2, "warnLevel");
  lua_pushinteger(l, LOG_LEVEL_INFO);
  lua_setfield(l, -2, "infoLevel");
  lua_pushinteger(l, LOG_LEVEL_DEBUG);
  lua_setfield(l, -2, "debugLevel");
  lua_pushinteger(l, LOG_LEVEL_TRACE);
  lua_setfield(l, -2, "traceLevel");

  return 1;
}
