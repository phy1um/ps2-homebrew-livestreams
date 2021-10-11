#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <draw.h>
#include <dma.h>
#include <graph.h>

#include "log.h"

#include "script.h"

#define INIT_SCRIPT "host:script/draw.lua"

static int ps2luaprog_start_nil(lua_State *l) {
  info("default start...");
  return 0;
}

static int ps2luaprog_frame_nil(lua_State *l) {
  return 0;
}

int ps2luaprog_init(lua_State *l) {
  lua_createtable(l, 0, 2);
  lua_pushcfunction(l, ps2luaprog_start_nil);
  lua_setfield(l, -2, "start");
  lua_pushcfunction(l, ps2luaprog_frame_nil);
  lua_setfield(l, -2, "frame");
  lua_setglobal(l, "PS2PROG");
  return 0;
}

int ps2luaprog_onstart(lua_State *l) {
  lua_getglobal(l, "PS2PROG"); 
  lua_pushstring(l, "start");
  lua_gettable(l, -2);
  int type = lua_type(l, -1);
  info("start fn has type :: %s (%d)", lua_typename(l, type), type);
  int rc = lua_pcall(l, 0, 0, 0);
  if ( rc ) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (start event) -- %s", err);
  }

  return 0;
}

int ps2luaprog_onframe(lua_State *l) {
  lua_getglobal(l, "PS2PROG"); 
  lua_pushstring(l, "frame");
  lua_gettable(l, -2);
  int type = lua_type(l, -1);
  info("frame fn has type :: %s (%d)", lua_typename(l, type), type);
  int rc = lua_pcall(l, 0, 0, 0);

  if ( rc ) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (frame event) -- %s", err);
  }

  return 0;
}

int ps2luaprog_is_running(lua_State *l) {
  return 1;
}

int main(int argc, char *argv[]) {
  info("startup");
  struct lua_State *L;
  L = luaL_newstate();
  if ( !L ) {
    logerr("failed to start lua state");
    return -1;
  }
  luaL_openlibs(L);

  ps2luaprog_init(L);
  dma_lua_init(L);
  gs_lua_init(L);


  //TODO: better abstraction for drawlua_*
  drawlua_init(L);

  info("finished lua state setup");

  info("loading file " INIT_SCRIPT);

  int rc = luaL_loadfile(L, INIT_SCRIPT);
  if ( rc == LUA_ERRSYNTAX ) {
    logerr("failed to load " INIT_SCRIPT ": syntax error");
    const char *err = lua_tostring(L, -1);
    logerr("err: %s", err);
    return -1;
  }
  else if ( rc == LUA_ERRMEM ) {
    logerr("faild to allocate memory for " INIT_SCRIPT);
    return -1;
  }
  else if ( rc == LUA_ERRFILE ) {
    logerr("could not open/read file " INIT_SCRIPT);
    return -1;
  }
  else if ( rc ) {
    logerr("unknown error loading " INIT_SCRIPT);
    return -1;
  }

  rc = lua_pcall(L, 0, 0, 0);
  if ( rc ) {
    const char *err = lua_tostring(L, -1);
    logerr("lua execution error -- %s", err);
    return -1;
  }

  ps2luaprog_onstart(L);
  while( ps2luaprog_is_running(L) ) {
    dma_wait_fast();
    info("ON FRAME");
    ps2luaprog_onframe(L);
    // info("WAIT DRAW");
    draw_wait_finish();
    // info("WAIT VSYNC");
    graph_wait_vsync();
  }
  info("main loop ended");
}
