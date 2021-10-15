#include "drawstate.h"
#include "ps2draw.h"
#include "script.h"
#include <lua.h>

#include <graph.h>
#include <gs_psm.h>

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
  drawstate_camera_step(x, y, z);
}

static int lua_camera_jump(lua_State *L) {
  double x = lua_tonumber(L, 1);
  double y = lua_tonumber(L, 2);
  double z = lua_tonumber(L, 3);
  drawstate_camera_jump(x, y, z);
}

int lua_drawstate_init(lua_State *L) {
  LUA_DEF(gs_init);
  LUA_DEF(camera_step);
  LUA_DEF(camera_jump);
  return 0;
}

int lua_drawstate_constant_table(lua_State *L) {
  lua_createtable(L, 0, 4);
  FIELD_INT("INTERLACED", GRAPH_MODE_INTERLACED);
  FIELD_INT("NON_INTERLACED", GRAPH_MODE_NONINTERLACED);
  FIELD_INT("PSM32", GS_PSM_32);
  FIELD_INT("PSMZ24", GS_PSMZ_24);
  lua_setglobal(L, "GS");
  return 1;
}

// GS.interlaced / nonInterlaced
//
int lua_drawstate_new(lua_State *L, int w, int h, int i) {
  lua_createtable(L, 0, 4);
  FIELD_INT("interlace", i);
  FIELD_INT("width", w);
  FIELD_INT("height", h);
  FIELD_INT("ztest", 1);
  FIELD_CFUNC("init", lua_gs_init);
  return 1;
}
