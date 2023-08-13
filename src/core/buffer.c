#include <lauxlib.h>
#include <lua.h>

#include <draw.h>
#include <draw_tests.h>
#include <gif_tags.h>
#include <gs_gp.h>
#include <gs_psm.h>
#include <tamtypes.h>

#include <stdlib.h>
#include <string.h>

#include <p2g/log.h>
#include <p2g/script.h>

static int buffer_alloc(lua_State *l);
static int buffer_gcalloc(lua_State *l);
static int drawlua_start_frame(lua_State *l);
static int drawlua_end_frame(lua_State *l);

static int buffer_pushint(lua_State *l) {
  // buf:pushint(i)
  int value = lua_tointeger(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *)lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  lua_pushstring(l, "size");
  lua_gettable(l, 1);
  int size = lua_tointeger(l, -1);
  if (head >= size) {
    logerr("lua buffer pushint overflow: size=%d, head=%d", size, head);
    return 0;
  }
  // ASSUME 4byte int
  if (head % 4 != 0) {
    // TODO(Tom Marks): manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head % 4);
    return 0;
  }
  // info("db write int %d @ %d", value, head);
  ptr[head / 4] = value;
  // info("db head -> %d", head+4);
  lua_pushinteger(l, head + 4);
  lua_setfield(l, 1, "head");
  return 0;
}

static int buffer_pushfloat(lua_State *l) {
  // buf:pushfloat(i)
  float value = (float)lua_tonumber(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  float *ptr = (float *)lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  // TODO(Tom Marks): check size
  // ASSUME 4byte int
  if (head % 4 != 0) {
    // TODO(Tom Marks): manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head % 4);
    return 0;
  }
  // info("db write int %d @ %d", value, head);
  ptr[head / 4] = value;
  // info("db head -> %d", head+4);
  lua_pushinteger(l, head + 4);
  lua_setfield(l, 1, "head");
  return 0;
}

static int buffer_settex(lua_State *l) {
  // buf:settex(reg, tbp, tbw, psm, tw, th, tcc, tfx)
  int reg = lua_tointeger(l, 2);
  int tbp = lua_tointeger(l, 3);
  int tbw = lua_tointeger(l, 4);
  int psm = lua_tointeger(l, 5);
  int tw = lua_tointeger(l, 6);
  int th = lua_tointeger(l, 7);
  int tcc = lua_tointeger(l, 8);
  int tfx = lua_tointeger(l, 9);

  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *)lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  // TODO(Tom Marks): check size
  // ASSUME 4byte int
  if (head % 4 != 0) {
    // TODO(Tom Marks): manually bitmask etc
    logerr("cannot write int to buffer, head%%4 != 0 (%d|%d)", head, head % 4);
    return 0;
  }

  int *base = ptr + (head / 4);

  int v1 = tbp | (tbw << 14) | (psm << 20) | (tw << 26) | ((th & 0x3) << 30);
  // TODO(Tom Marks): 0x5??? i must mean 0x4
  int v2 = ((th & 0xc) >> 2) | (tcc << 1) | (tfx << 2);
  int v3 = 0x6 + reg;
  int v4 = 0;
  *(base) = v1;
  *(base + 1) = v2;
  *(base + 2) = v3;
  *(base + 3) = v4;

  trace("write tex0 :: %08x %08x %08x %08x", v1, v2, v3, v4);
  // info("head -> %d", head+16);

  lua_pushinteger(l, head + 16);
  lua_setfield(l, 1, "head");

  // info("backtrack tex0 :: %08x %08x %08x %08x", base[0], base[1], base[2],
  // base[3]);
  return 0;
}

static int buffer_pushmiptbp(lua_State *l) {
  // buf:pushMipTbp(p1, w1, p2, w2, p3, w3)
  int p1 = lua_tointeger(l, 2);
  int w1 = lua_tointeger(l, 3);
  int p2 = lua_tointeger(l, 4);
  int w2 = lua_tointeger(l, 5);
  int p3 = lua_tointeger(l, 6);
  int w3 = lua_tointeger(l, 7);

  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *)lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);

  if (head % 4 != 0) {
    logerr("buffer head must be =0%%4, got %d", head % 4);
    lua_pushstring(l, "buffer write failed");
    lua_error(l);
    return 1;
  }

  int v1 = p1 | (w1 << 14) | (p2 << 20);
  int overflow = (p2 & 0x1000) >> 12;
  int v2 = overflow | (w2 << 2) | (p3 << 8) | (w3 << 22);

  (ptr + head)[0] = v1;
  (ptr + head)[1] = v2;

  lua_pushinteger(l, head + 8);
  lua_setfield(l, 1, "head");
  return 0;
}

static int buffer_copy(lua_State *l) {
  // buf:copy(targetBuffer, targetOffset, fromOffset, numBytes)
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

static int buffer_slice(lua_State *l) {
  // buf:slice(offset, len) where buffer.size <= offset+len
  int offset = lua_tointeger(l, 2);
  int slice_length = lua_tointeger(l, 3);
  lua_getfield(l, 1, "size");
  int buffer_length = lua_tointeger(l, -1);
  if (offset+slice_length > buffer_length) {
    lua_pushstring(l, "slice longer than buffer");
    lua_error(l);
    return 1;
  }
  lua_getfield(l, 1, "ptr");
  char *buf = lua_touserdata(l, -1);
  char *slice_start = buf+offset;

  lua_createtable(l, 0, 2);
  lua_pushinteger(l, slice_length);
  lua_setfield(l, -2, "size"); lua_pushinteger(l, 0);
  lua_setfield(l, -2, "head");
  lua_pushlightuserdata(l, slice_start);
  lua_setfield(l, -2, "ptr");
  lua_pushinteger(l, (int)slice_start);
  lua_setfield(l, -2, "addr");
  lua_pushvalue(l, 1);
  lua_setfield(l, -2, "_owner");
  luaL_getmetatable(l, "ps2.buffer");
  lua_setmetatable(l, -2);
  return 1;
}

static int buffer_read(lua_State *l) {
  // buf:read(index)
  int index = lua_tointeger(l, 2);
  if (index % 4 != 0) {
    return luaL_error(l, "read %d not /4", index);
  }
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  int *ptr = (int *)lua_touserdata(l, -1);
  int res = ptr[index / 4];
  lua_pushinteger(l, res);
  return 1;
}

static int buffer_getfloat(lua_State *l) {
  // buf:getfloat(index)
  int index = lua_tointeger(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  float *ptr = (float *)lua_touserdata(l, -1);
  float res = ptr[index];
  lua_pushnumber(l, res);
  return 1;
}

static int buffer_setfloat(lua_State *l) {
  // buf:setfloat(index, value)
  int index = lua_tointeger(l, 2);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  float *ptr = (float *)lua_touserdata(l, -1);
  float value = lua_tonumber(l, 3);
  ptr[index] = value;
  return 0;
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
  int *ptr = (int *)lua_touserdata(l, -1);
  ptr[index / 4] = value;
  return 0;
}

static int buffer_print(lua_State *l) {
  // buf:print()
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  unsigned char *ptr = (unsigned char *)lua_touserdata(l, -1);
  if (ptr == 0) {
    logerr("cannot print buffer with NULL pointer");
    return 0;
  }
  lua_pushstring(l, "size");
  lua_gettable(l, 1);
  int size = lua_tointeger(l, -1);
  if (size == 0) {
    info("BUFFER = []");
    return 0;
  }
  info("BUFFER(%p): ", ptr);
  for (int i = 0; i < size - 7; i += 8) {
    info(" %x %x %x %x   %x %x %x %x", ptr[i], ptr[i + 1], ptr[i + 2],
         ptr[i + 3], ptr[i + 4], ptr[i + 5], ptr[i + 6], ptr[i + 7]);
  }
  for (int i = 0; i < size % 8; i++) {
    printf(" %d", ptr[i]);
  }
  return 0;
}

#ifdef TRACE_LUA_BUFFER_GC
int buffer_on_gc(lua_State *l) {
  trace("some buffer was GC'd");
  return 0;
}
#endif

int draw_lua_init(lua_State *l) {
  luaL_newmetatable(l, "ps2.buffer");

#ifdef TRACE_LUA_BUFFER_GC
  lua_pushcfunction(l, buffer_on_gc);
  lua_setfield(l, -2, "__gc");
#endif

  lua_createtable(l, 0, 5);

  lua_pushcfunction(l, buffer_pushint);
  lua_setfield(l, -2, "pushint");

  lua_pushcfunction(l, buffer_pushfloat);
  lua_setfield(l, -2, "pushfloat");

  lua_pushcfunction(l, buffer_settex);
  lua_setfield(l, -2, "settex");

  lua_pushcfunction(l, buffer_pushmiptbp);
  lua_setfield(l, -2, "setMipTbp");

  // NOTE: this is a hack
  lua_pushcfunction(l, drawlua_start_frame);
  lua_setfield(l, -2, "frameStart");
  lua_pushcfunction(l, drawlua_end_frame);
  lua_setfield(l, -2, "frameEnd");

  lua_pushcfunction(l, buffer_copy);
  lua_setfield(l, -2, "copy");
  lua_pushcfunction(l, buffer_slice);
  lua_setfield(l, -2, "slice");

  lua_pushcfunction(l, buffer_print);
  lua_setfield(l, -2, "print");

  lua_pushcfunction(l, buffer_read);
  lua_setfield(l, -2, "read");
  lua_pushcfunction(l, buffer_write);
  lua_setfield(l, -2, "write");
  lua_pushcfunction(l, buffer_getfloat);
  lua_setfield(l, -2, "getFloat");
  lua_pushcfunction(l, buffer_setfloat);
  lua_setfield(l, -2, "setFloat");

  lua_setfield(l, -2, "__index");
  lua_pop(l, 1);

  lua_createtable(l, 0, 1);
  lua_pushcfunction(l, buffer_alloc);
  lua_setfield(l, -2, "alloc");
  lua_pushcfunction(l, buffer_gcalloc);
  lua_setfield(l, -2, "gcAlloc");

  return 1;
}

zbuffer_t zb = {0};
static int drawlua_start_frame(lua_State *l) {
  // drawbuffer is arg #1
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);

  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  char *ptr = (char *)lua_touserdata(l, -1);

  int width = lua_tointeger(l, 2);
  int height = lua_tointeger(l, 3);
  int r = lua_tointeger(l, 4);
  int g = lua_tointeger(l, 5);
  int b = lua_tointeger(l, 6);

  float halfw = width / 2.f;
  float halfh = height / 2.f;

  qword_t *q = (qword_t *)(ptr + head);
  q = draw_disable_tests(q, 0, &zb);
  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, ZTEST_METHOD_ALLPASS),
              GS_REG_TEST);
  q++;
  q = draw_clear(q, 0, 2048.0f - halfw, 2048.0f - halfh, width, height, r, g,
                 b);

  head = (char *)q - ptr;
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
  char *ptr = (char *)lua_touserdata(l, -1);

  qword_t *q = (qword_t *)(ptr + head);
  q = draw_finish(q);

  head = (char *)q - ptr;
  // info("db head -> %d", head);
  lua_pushinteger(l, head);
  lua_setfield(l, 1, "head");
  return 0;
}

static int drawbuffer_free(lua_State *l) { return 0; }

static int buffer_alloc(lua_State *l) {
  int size = lua_tointeger(l, 1);
  trace("allocating buffer for lua, size = %d", size);
  void *buf = malloc(size);

  lua_createtable(l, 0, 2);
  lua_pushinteger(l, size);
  lua_setfield(l, -2, "size");
  lua_pushinteger(l, 0);
  lua_setfield(l, -2, "head");
  lua_pushlightuserdata(l, buf);
  lua_setfield(l, -2, "ptr");
  lua_pushinteger(l, (int)buf);
  lua_setfield(l, -2, "addr");
  luaL_getmetatable(l, "ps2.buffer");
  lua_setmetatable(l, -2);

  return 1;
}

static int buffer_gcalloc(lua_State *l) {
  int size = lua_tointeger(l, 1);
  trace("allocating GC buffer for lua, size = %d", size);

  lua_createtable(l, 0, 2);
  // returns a pointer to the memory block and pushes to the stack.
  void *buf = lua_newuserdata(l, size);
  lua_setfield(l, -2, "_gc_ref");
  lua_pushinteger(l, size);
  lua_setfield(l, -2, "size");
  lua_pushinteger(l, 0);
  lua_setfield(l, -2, "head");
  lua_pushlightuserdata(l, buf);
  lua_setfield(l, -2, "ptr");
  lua_pushinteger(l, (int)buf);
  lua_setfield(l, -2, "addr");
  luaL_getmetatable(l, "ps2.buffer");
  lua_setmetatable(l, -2);

  return 1;
}
