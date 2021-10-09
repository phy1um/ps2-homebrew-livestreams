#include <lua.h>

#include <draw.h>
#include <graph.h>
#include <inttypes.h>
#include <gs_psm.h>
#include <dma.h>

#include <stdlib.h>
#include <string.h>

#include "script.h"
#include "log.h"

static int gslua_clear_col(lua_State *l) {
  lua_pushstring(l, "state");
  lua_gettable(l, 1);
  struct gs_state *st = (struct gs_state *) lua_touserdata(l, -1);
  lua_pop(l, 1);
  if ( !st ) {
    logerr("GS state was NULL"); 
    return 0;
  }
  int r = lua_tointeger(l, 2);
  int g = lua_tointeger(l, 3);
  int b = lua_tointeger(l, 4); 
  st->clear_r = r;
  st->clear_g = g;
  st->clear_b = b;
  return 0;
}

static int gslua_vram_alloc(lua_State *l) {
  int width = lua_tointeger(l, 2);
  int height = lua_tointeger(l, 3);
  int format = lua_tointeger(l, 4);
  int addr = graph_vram_allocate(width, height, format, GRAPH_ALIGN_PAGE);
  lua_createtable(l, 0, 4);
  lua_pushinteger(l, addr);
  lua_setfield(l, -2, "address");
  lua_pushinteger(l, width);
  lua_setfield(l, -2, "width");
  lua_pushinteger(l, height);
  lua_setfield(l, -2, "height");
  lua_pushinteger(l, format);
  lua_setfield(l, -2, "format");
  return 1;
}

static int gslua_set_buffers(lua_State *l) {
  // get fields from SELF argument (#1)
  lua_pushstring(l, "state");
  lua_gettable(l, 1);
  struct gs_state *st = (struct gs_state *) lua_touserdata(l, -1);
  lua_pop(l, 1);
  // get fields from FB argument (#2)
  lua_pushstring(l, "width");
  lua_gettable(l, 2);
  int fb_width = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "height");
  lua_gettable(l, 2);
  int fb_height = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "address");
  lua_gettable(l, 2);
  int fb_addr = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushstring(l, "format");
  lua_gettable(l, 2);
  int fb_fmt = lua_tointeger(l, -1);
  lua_pop(l, 1);
  // get fields from ZB argument (#3)
  lua_pushstring(l, "address");
  lua_gettable(l, 3);
  int zb_addr = lua_tointeger(l, -1);
  lua_pushstring(l, "format");
  lua_gettable(l, 3);
  int zb_fmt = lua_tointeger(l, -1);
  lua_pop(l, 1);

  if (!st) {
    // TODO: error
  }

  st->fb.address = fb_addr;
  st->fb.width = fb_width;
  st->fb.height = fb_height;
  st->fb.psm = fb_fmt;
  st->fb.mask = 0;
  st->zb.address = zb_addr;
  st->zb.zsm = zb_fmt;
  st->zb.method = ZTEST_METHOD_GREATER;
  st->zb.mask = 0;
  graph_set_framebuffer_filtered(st->fb.address, fb_width, fb_fmt, 0, 0);
  graph_enable_output();

  // init draw state
  qword_t *head = malloc(20 * 16);
  memset(head, 0, 20*16);
  qword_t *q = head;
  q = draw_setup_environment(q, 0, &st->fb, &st->zb);
  q = draw_primitive_xyoffset(q, 0, 2048 - (st->fb.width / 2),
                              2048 - (st->fb.height / 2));
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, head, q - head, 0,
                          0);
  draw_wait_finish();
  free(head);

  return 0;
}

static int gslua_new_state(lua_State *l) {
  int width = lua_tointeger(l, 1); 
  int height = lua_tointeger(l, 2); 
  int interlace = lua_tointeger(l, 3);
  int mode = lua_tointeger(l, 4);
  // create table with GS state
  lua_createtable(l, 0, 5);
  lua_newuserdata(l, sizeof(struct gs_state));
  lua_setfield(l, -2, "state");
  lua_pushinteger(l, width);
  lua_setfield(l, -2, "width");
  lua_pushinteger(l, height);
  lua_setfield(l, -2, "height");
  lua_pushinteger(l, interlace);
  lua_setfield(l, -2, "interlace");
  lua_pushinteger(l, mode);
  lua_setfield(l, -2, "mode");

  lua_pushcfunction(l, gslua_vram_alloc);
  lua_setfield(l, -2, "alloc");
  lua_pushcfunction(l, gslua_set_buffers);
  lua_setfield(l, -2, "setBuffers");
  lua_pushcfunction(l, gslua_clear_col);
  lua_setfield(l, -2, "clearColour");
  // initialize GS with no output
  graph_disable_output();
  graph_set_mode(interlace, mode, GRAPH_MODE_FIELD, GRAPH_DISABLE);
  graph_set_screen(0, 0, width, height);
  graph_set_bgcolor(0, 0, 0);
  return 1;
}

#define bind(n, b) \
  lua_pushinteger(l, b);\
  lua_setfield(l, -2, n)
int gs_lua_init(lua_State *l) {
  lua_createtable(l, 0, 16);
  lua_pushcfunction(l, gslua_new_state);
  lua_setfield(l, -2, "newState");

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

  // bind this table to global name GS, nothing left on stack
  lua_setglobal(l, "GS");
  return 0;
}


