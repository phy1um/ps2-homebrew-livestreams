#include <lua.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <inttypes.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <p2g/log.h>
#include <p2g/gs.h>

#include "../gs_state.h"

static int gslua_set_buffers(lua_State *l) {
  struct gs_state *st = GS_STATE;
  if (!st) {
    lua_pushstring(l, "GS state was NULL");
    lua_error(l);
    return 1;
  }

  // get fields from FB argument (#1, #2)
  lua_pushstring(l, "width");
  lua_gettable(l, 1);
  int fb_width = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "height");
  lua_gettable(l, 1);
  int fb_height = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "address");
  lua_gettable(l, 1);
  int fb1_addr = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "format");
  lua_gettable(l, 1);
  int fb_fmt = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "address");
  lua_gettable(l, 2);
  int fb2_addr = lua_tointeger(l, -1);
  lua_pop(l, 1);

  // get fields from ZB argument (#4)
  lua_pushstring(l, "address");
  lua_gettable(l, 3);
  int zb_addr = lua_tointeger(l, -1);
  lua_pushstring(l, "format");
  lua_gettable(l, 3);
  int zb_fmt = lua_tointeger(l, -1);
  lua_pop(l, 1);

  gs_set_fields(fb_width, fb_height, fb_fmt, zb_fmt, fb1_addr, fb2_addr, zb_addr);
  return 0;
}

static int gslua_set_output(lua_State *l) {
  int width = lua_tointeger(l, 1);
  int height = lua_tointeger(l, 2);
  int interlace = lua_tointeger(l, 3);
  int mode = lua_tointeger(l, 4);
  gs_set_output(width, height, interlace, mode, GRAPH_MODE_FIELD, GRAPH_DISABLE);
  return 0;
}

#define bind(n, b)                                                             \
  lua_pushinteger(l, b);                                                       \
  lua_setfield(l, -2, n)

int gs_lua_init(lua_State *l) {
  lua_createtable(l, 0, 16);
  lua_pushcfunction(l, gslua_set_output);
  lua_setfield(l, -2, "setOutput");
  lua_pushcfunction(l, gslua_set_buffers);
  lua_setfield(l, -2, "setBuffers");

  bind("PSM4", GS_PSM_4);
  bind("PSM4HL", GS_PSM_4HL);
  bind("PSM4HH", GS_PSM_4HH);
  bind("PSM8", GS_PSM_8);
  bind("PSM8H", GS_PSM_8H);
  bind("PSM16", GS_PSM_16);
  bind("PSM16S", GS_PSM_16S);
  bind("PSM24", GS_PSM_24);
  bind("PSMPS24", GS_PSM_PS24);
  bind("PSM32", GS_PSM_32);

  bind("PSMZ16", GS_PSMZ_16);
  bind("PSMZ16S", GS_PSMZ_16S);
  bind("PSMZ24", GS_PSMZ_24);
  bind("PSMZ32", GS_PSMZ_32);

  bind("NONINTERLACED", 1);
  bind("INTERLACED", 1);

  bind("AUTO", GRAPH_MODE_AUTO);
  bind("NTSC", GRAPH_MODE_NTSC);
  bind("PAL", GRAPH_MODE_PAL);
  bind("HDTV_480P", GRAPH_MODE_HDTV_480P);
  bind("HDTV_576P", GRAPH_MODE_HDTV_576P);
  bind("HDTV_720P", GRAPH_MODE_HDTV_720P);

  return 1;
}
