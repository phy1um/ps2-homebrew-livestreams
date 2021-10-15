#include <lua.h>
#include <lauxlib.h>

#include <draw.h>
#include <tamtypes.h>

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "script.h"


static int drawlua_new_drawbuffer(lua_State *l);

static const unsigned int DRAW_BUFFER_MAX_SIZE = 5 * 1024;
char *static_draw_buffer;

static int buffer_pushint(lua_State *l) {
  int value = lua_tointeger(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *) lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  // TODO: check size
  // ASSUME 4byte int
  if (head%4 != 0) {
    // TODO: manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head%4);
    return 0;
  }
  // info("db write int %d @ %d", value, head);
  ptr[head/4] = value;
  // info("db head -> %d", head+4);
  lua_pushinteger(l, head+4);
  lua_setfield(l, 1, "head");
  return 0;
}

static int buffer_pushfloat(lua_State *l) {
  float value = (float) lua_tonumber(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  float *ptr = (float *) lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  // TODO: check size
  // ASSUME 4byte int
  if (head%4 != 0) {
    // TODO: manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head%4);
    return 0;
  }
  // info("db write int %d @ %d", value, head);
  ptr[head/4] = value;
  // info("db head -> %d", head+4);
  lua_pushinteger(l, head+4);
  lua_setfield(l, 1, "head");
  return 0;
}


static int buffer_settex(lua_State *l) {
  int reg= lua_tointeger(l, 2);
  int tbp= lua_tointeger(l, 3);
  int tbw= lua_tointeger(l, 4);
  int psm= lua_tointeger(l, 5);
  int tw = lua_tointeger(l, 6);
  int th = lua_tointeger(l, 7);
  int tcc= lua_tointeger(l, 8);
  int tfx= lua_tointeger(l, 9);

  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *) lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  // TODO: check size
  // ASSUME 4byte int
  if (head%4 != 0) {
    // TODO: manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head%4);
    return 0;
  }

  int *base = ptr + (head/4);

  int v1 = tbp | (tbw<<14) | (psm<<20) | (tw<<26) | ((th&0x3) << 30);
  // TODO: 0x5??? i must mean 0x4
  int v2 = ((th&0x5)>>2) | (tcc<<1) | (tfx<<2);
  int v3 = 0x6+reg;
  int v4 = 0;
  *(base) = v1;
  *(base+1) = v2;
  *(base+2) = v3;
  *(base+3) = v4;

  // info("write tex0 :: %08x %08x %08x %08x", v1, v2, v3, v4);
  // info("head -> %d", head+16);

  lua_pushinteger(l, head+16);
  lua_setfield(l, 1, "head");

  // info("backtrack tex0 :: %08x %08x %08x %08x", base[0], base[1], base[2], base[3]);
  return 0;
}

static int buffer_pushmiptbp(lua_State *l) {
  int p1 = lua_tointeger(l, 2);
  int w1 = lua_tointeger(l, 3);
  int p2 = lua_tointeger(l, 4);
  int w2 = lua_tointeger(l, 5);
  int p3 = lua_tointeger(l, 6);
  int w3 = lua_tointeger(l, 7);

  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *) lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  
  if (head%4 != 0) {
    logerr("buffer head must be =0%%4, got %d", head%4);
    lua_pushstring(l, "buffer write failed");
    lua_error(l);
    return 1;
  }

  int v1 = p1 | (w1 << 14) | (p2 << 20);
  int overflow = (p2&0x1000) >> 12;
  int v2 = overflow | (w2 << 2) | (p3 << 8) | (w3 << 22);

  (ptr+head)[0] = v1;
  (ptr+head)[1] = v2;

  lua_pushinteger(l, head+8);
  lua_setfield(l, 1, "head");
  return 0;
}

static int buffer_copy(lua_State *l) {
  // arg 1 = buffer from
  // arg 2 = buffer TO
  // arg 3 = buffer TO offset
  // arg 4 = buffer FROM offset
  // arg 5 = n bytes
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  char *ptr_from = lua_touserdata(l, -1);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 2);
  char *ptr_to = lua_touserdata(l, -1);

  int to_offset = lua_tointeger(l, 3);
  int from_offset = lua_tointeger(l, 4);
  int n = lua_tointeger(l, 5);

  memcpy(ptr_to + to_offset, ptr_from + from_offset, n);

  return 0;
}

static int buffer_read(lua_State *l) {
  int index = lua_tointeger(l, 2);
  if (index % 4 != 0) {
    lua_pushstring(l, "invalid read index, not ==0 %%4");
    lua_error(l);
    return 1;
  }
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *) lua_touserdata(l, -1);
  int res = ptr[index/4];
  lua_pushinteger(l,res);
  return 1;
}

static int buffer_write(lua_State *l) {
  int index = lua_tointeger(l, 2);
  if (index % 4 != 0) {
    lua_pushstring(l, "invalid read index, not ==0 %%4");
    lua_error(l);
    return 1;
  }
  int value = lua_tointeger(l, 3);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *) lua_touserdata(l, -1);
  ptr[index/4] = value;
  return 0;
}

int drawlua_init(lua_State *l) {
  luaL_newmetatable(l, "ps2.buffer");  
  lua_createtable(l, 0, 5);

  lua_pushcfunction(l, buffer_pushint);
  lua_setfield(l, -2, "pushint");

  lua_pushcfunction(l, buffer_pushfloat);
  lua_setfield(l, -2, "pushfloat");


  lua_pushcfunction(l, buffer_settex);
  lua_setfield(l, -2, "settex");

  lua_pushcfunction(l, buffer_pushmiptbp);
  lua_setfield(l, -2, "setMipTbp");

  lua_pushcfunction(l, buffer_copy);
  lua_setfield(l, -2, "copy");

  lua_pushcfunction(l, buffer_read);
  lua_setfield(l, -2, "read");  
  lua_pushcfunction(l, buffer_write);
  lua_setfield(l, -2, "write");
  lua_setfield(l, -2, "__index");
  lua_pop(l, 1);

  lua_createtable(l, 0, 8);
  lua_pushcfunction(l, drawlua_new_drawbuffer);
  lua_setfield(l, -2, "getDrawBuffer");
  lua_setglobal(l, "RM");

  info("allocating static draw buffer");
  static_draw_buffer = malloc(DRAW_BUFFER_MAX_SIZE);

  return 0;
}


static int drawlua_start_frame(lua_State *l) {
  // drawbuffer is arg #1
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  /*
  lua_pushstring(l, "size");
  lua_gettable(l, 1);
  int size = lua_tointeger(l, -1);
  */
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  char *ptr = (char *) lua_touserdata(l, -1);

  // gs is arg #2
  lua_pushstring(l, "state");
  lua_gettable(l, 2);
  struct gs_state *st = (struct gs_state *) lua_touserdata(l, -1);
  lua_pushstring(l, "width");
  lua_gettable(l, 2);
  int width = lua_tointeger(l, -1);
  lua_pushstring(l, "height");
  lua_gettable(l, 2);
  int height = lua_tointeger(l, -1);

  float halfw = st->fb.width / 2.f;
  float halfh = st->fb.height / 2.f;

  // info("clear screen :: (%d, %d, %d)", st->clear_r, st->clear_g, st->clear_b);

  qword_t *q = (qword_t *) (ptr + head);
  q = draw_disable_tests(q, 0, &st->zb);
  q = draw_clear(q, 0, 2048.0f - halfw, 2048.0f - halfh, width, height,
    st->clear_r, st->clear_g, st->clear_b);
  //q = draw_enable_tests(q, 0, &st->zb);

  head = (char*)q - ptr;
  // info("db head -> %d", head);
  lua_pushinteger(l, head);
  lua_setfield(l, 1, "head");
  return 0;
}

static int drawlua_end_frame(lua_State *l) {
  // drawbuffer is arg #1
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  /*
  lua_pushstring("size");
  lua_gettable(l, 1);
  int size = lua_tointeger(l, -1);
  */
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  char *ptr = (char *) lua_touserdata(l, -1);

  qword_t *q = (qword_t *) (ptr + head);
  q = draw_finish(q);

  head = (char*)q - ptr;
  // info("db head -> %d", head);
  lua_pushinteger(l, head);
  lua_setfield(l, 1, "head");
  return 0;
}

static int drawbuffer_free(lua_State *l) {
  return 0; 
}

// TODO: document this can only be called ONCE
static int drawlua_new_drawbuffer(lua_State *l) {
  int size = lua_tointeger(l, 1);
  if ( size >= DRAW_BUFFER_MAX_SIZE ) {
    logerr("invalid drawbuffer size: %d must be smaller than %d", size, DRAW_BUFFER_MAX_SIZE);
    lua_pushstring(l, "drawbuffer size is too big");
    lua_error(l);
    return 0;
  }
  lua_createtable(l, 0, 5);
  lua_pushinteger(l, size);
  lua_setfield(l, -2, "size");
  lua_pushinteger(l, 0);
  lua_setfield(l, -2, "head");
  char *buf = static_draw_buffer;
  lua_pushlightuserdata(l, buf);
  lua_setfield(l, -2, "ptr");
  lua_pushcfunction(l, drawlua_start_frame);
  lua_setfield(l, -2, "frameStart");
  lua_pushcfunction(l, drawlua_end_frame);
  lua_setfield(l, -2, "frameEnd");
  lua_pushcfunction(l, drawbuffer_free);
  lua_setfield(l, -2, "free");
  luaL_getmetatable(l, "ps2.buffer");
  lua_setmetatable(l, -2);
  return 1;
}
