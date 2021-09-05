#include <lua.h>
#include "script.h"
#include "ps2draw.h"
#include "drawstate.h"

static int lua_gs_init(lua_State *L) {
  int width = lua_tointeger(L, 1);
  int height = lua_tointeger(L, 2);
  int interlace = lua_tointeger(L, 3);
  drawstate_init(width, height, interlace);
}

static int lua_camera_step(lua_State *L) {
  double x = lua_tonumber(L, 1);
  double y = lua_tonumber(L, 2);
  double z = lua_tonumber(L, 3);
  drawstate_camera_step(x,y,z);
}

static int lua_camera_jump(lua_State *L) {
  double x = lua_tonumber(L, 1);
  double y = lua_tonumber(L, 2);
  double z = lua_tonumber(L, 3);
  drawstate_camera_jump(x,y,z);
}

int lua_drawstate_init(lua_State *L) {
  LUA_DEF(gs_init);
  LUA_DEF(camera_step);
  LUA_DEF(camera_jump);
}


