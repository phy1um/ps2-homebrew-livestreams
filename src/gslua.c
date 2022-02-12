#include <lua.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <inttypes.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "script.h"

struct gs_state {
  framebuffer_t fb[2];
  zbuffer_t zb;
  int ctx;
};

static struct gs_state *st;
static qword_t *flip_buffer;

int gs_init() {
  info("init GS -- no framebuffer");
  st = calloc(1, sizeof(struct gs_state));
  flip_buffer = memalign(64, 10 * 16);
  return 0;
}

int gs_flip() {
  trace("GS FLIP START");
  memset(flip_buffer, 0, 10 * 16);
  framebuffer_t *fb = &st->fb[st->ctx];
  graph_set_framebuffer_filtered(fb->address, fb->width, fb->psm, 0, 0);
  st->ctx ^= 1;
  trace("GS BUILD FLIP BUFFER");
  qword_t *q = flip_buffer;
  q = draw_framebuffer(q, 0, &st->fb[st->ctx]);
  q = draw_finish(q);
  dma_wait_fast();
  trace("GS SEND FLIP BUFFER");
  dma_channel_send_normal(DMA_CHANNEL_GIF, flip_buffer, q - flip_buffer, 0, 0);
  trace("GS WAIT FLIP BUFFER DRAW FINISH");
  draw_wait_finish();
  trace("GS FLIP DONE");
  return 0;
}

static int gslua_set_buffers(lua_State *l) {
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

  st->fb[0].address = fb1_addr;
  st->fb[0].width = fb_width;
  st->fb[0].height = fb_height;
  st->fb[0].psm = fb_fmt;
  st->fb[0].mask = 0;
  st->fb[1].address = fb2_addr;
  st->fb[1].width = fb_width;
  st->fb[1].height = fb_height;
  st->fb[1].psm = fb_fmt;
  st->fb[1].mask = 0;

  st->zb.address = zb_addr;
  st->zb.zsm = zb_fmt;
  // st->zb.method = ZTEST_METHOD_GREATER_EQUAL;
  st->zb.method = ZTEST_METHOD_ALLPASS;
  st->zb.mask = 0;
  graph_set_framebuffer_filtered(fb2_addr, fb_width, fb_fmt, 0, 0);
  graph_enable_output();

  // init draw state
  qword_t *head = malloc(20 * 16);
  memset(head, 0, 20 * 16);
  qword_t *q = head;
  q = draw_setup_environment(q, 0, st->fb, &st->zb);
  q = draw_primitive_xyoffset(q, 0, 2048 - (fb_width / 2),
                              2048 - (fb_height / 2));
  q = draw_finish(q);
  print_buffer(head, q - head);
  dma_channel_send_normal(DMA_CHANNEL_GIF, head, q - head, 0, 0);
  draw_wait_finish();
  free(head);

  return 0;
}

static int gslua_set_output(lua_State *l) {
  int width = lua_tointeger(l, 1);
  int height = lua_tointeger(l, 2);
  int interlace = lua_tointeger(l, 3);
  int mode = lua_tointeger(l, 4);

  // initialize GS with no output
  graph_disable_output();
  graph_set_mode(interlace, mode, GRAPH_MODE_FIELD, GRAPH_DISABLE);
  graph_set_screen(0, 0, width, height);
  graph_set_bgcolor(0, 0, 0);
  return 1;
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

  // bind this table to global name GS, nothing left on stack
  lua_setglobal(l, "GS");
  return 0;
}
