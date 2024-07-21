#include <lua.h>
#include <lauxlib.h>

#include <math.h>

#include <p2g/bench.h>
#include <p2g/log.h>
#include <p2g/ps2luaprog.h>

int ps2luaprog_is_running = 0;

static int ps2luaprog_start_nil(lua_State *l) {
  info("default start...");
  return 0;
}

static int ps2luaprog_frame_nil(lua_State *l) { return 0; }

static int ps2lua_log2(lua_State *l) {
  int n = lua_tointeger(l, 1);
  float res = log2f(n);
  lua_pushnumber(l, res);
  return 1;
}

int ps2luaprog_set_log_level(lua_State *l) {
  log_output_level = lua_tointeger(l, 1);
  logdbg("updated log level = %d", log_output_level);
  return 0;
}

int ps2luaprog_get_log_level(lua_State *l) {
  lua_pushinteger(l, log_output_level);
  return 1;
}

int ps2luaprog_init(lua_State *l) {
  lua_createtable(l, 0, 2);
  lua_pushcfunction(l, ps2luaprog_start_nil);
  lua_setfield(l, -2, "start");
  lua_pushcfunction(l, ps2luaprog_frame_nil);
  lua_setfield(l, -2, "frame");
  lua_pushcfunction(l, ps2luaprog_set_log_level);
  lua_setfield(l, -2, "logLevel");
  lua_pushcfunction(l, ps2luaprog_get_log_level);
  lua_setfield(l, -2, "get_log_level");
  lua_setglobal(l, "PS2PROG");
  lua_pushcfunction(l, ps2lua_log2);
  lua_setglobal(l, "log2");
  return 0;
}

int ps2luaprog_onstart(lua_State *l) {
  BENCH_START(timer);
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "start");
  lua_gettable(l, -2);

  int rc = lua_pcall(l, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(l, -1);
    luaL_traceback(l, l, err, 1);
    const char *traceback = lua_tostring(l, -1);
    logerr("lua execution error (start event)\n%s", traceback);
    return rc;
  }
  BENCH_INFO(timer, " PS2PROG onstart in %f");

  return 0;
}

int ps2luaprog_onframe(lua_State *l) {
  trace("PS2LUAPROG.frame called from C");
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "frame");
  lua_gettable(l, -2);

  int rc = lua_pcall(l, 0, 0, 0);

  if (rc) {
    const char *err = lua_tostring(l, -1);
    luaL_traceback(l, l, err, 1);
    const char *traceback = lua_tostring(l, -1);
    logerr("lua execution error (frame event)\n%s", traceback);
    return rc;
  }

  return 0;
}
