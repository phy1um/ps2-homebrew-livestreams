#include <lua.h>

#include "../ps2math.h"
#include "../log.h"

#define buffer_null_check(p, s) \
  do { if (!p) { lua_pushstring(l, "got null buffer for " s); \
                lua_error(l); return 1; } }while(0)

#define get_buf(i, to, msg) \
  lua_pushstring(l, "ptr"); \
  lua_gettable(l, i); \
  float *to = lua_touserdata(l, -1); \
  buffer_null_check(to, msg " [" #i "]"); \
  ((void)0)


static int lua_add_vec2(lua_State *l) {
  get_buf(1, buf1, "copy vec");
  get_buf(2, buf2, "copy vec");

  buf1[0] += buf2[0];
  buf1[1] += buf2[1];

  return 0;
}

static int lua_sub_vec2(lua_State *l) {
  get_buf(1, buf1, "sub vec");
  get_buf(2, buf2, "sub vec");

  buf1[0] -= buf2[0];
  buf1[1] -= buf2[1];

  return 0;
}

// local c = copy(to, from)
static int lua_copy_vec2(lua_State *l) {
  get_buf(1, buf1, "copy vec");
  get_buf(2, buf2, "copy vec");

  buf1[0] = buf2[0];
  buf1[1] = buf2[1];

  return 0;
}

static int lua_length_vec2(lua_State *l) {
  get_buf(1, buf1, "length vec2");
  float result = p2m_vec2_length(buf1);
  lua_pushnumber(l, result);
  return 1;
}

static int lua_normalize_vec2(lua_State *l) {
  get_buf(1, buf1, "normalize vec2");
  float len = p2m_vec2_length(buf1);
  buf1[0] /= len;
  buf1[1] /= len;
  return 0;
}

static int lua_dot_vec2(lua_State *l) {
  get_buf(1, buf1, "dot vec2");
  get_buf(2, buf2, "dot vec2");
  float dot = p2m_vec2_dot(buf1, buf2);
  lua_pushnumber(l, dot);
  return 1;
}

static int lua_rotate_vec2(lua_State *l) {
  get_buf(1, v, "rotate vec2");
  float angle = lua_tonumber(l, 2);
  p2m_vec2_rotate(v, angle);
  return 0;
}

static int lua_scale_vec2(lua_State *l) {
  get_buf(1, buf1, "scale vec2");
  float scalar = lua_tonumber(l, 2);
  p2m_vec2_scale(buf1, scalar);
  return 0;
}

int vec2lua_init(lua_State *l) {
  lua_getglobal(l, "MATH_C_LIB");
  lua_pushcfunction(l, lua_add_vec2);
  lua_setfield(l, -2, "addVec2");
  lua_pushcfunction(l, lua_sub_vec2);
  lua_setfield(l, -2, "subVec2");
  lua_pushcfunction(l, lua_copy_vec2);
  lua_setfield(l, -2, "copyVec2");
  lua_pushcfunction(l, lua_length_vec2);
  lua_setfield(l, -2, "lenVec2");
  lua_pushcfunction(l, lua_normalize_vec2);
  lua_setfield(l, -2, "normalizeVec2");
  lua_pushcfunction(l, lua_dot_vec2);
  lua_setfield(l, -2, "dotVec2");
  lua_pushcfunction(l, lua_rotate_vec2);
  lua_setfield(l, -2, "rotateVec2");
  lua_pushcfunction(l, lua_scale_vec2);
  lua_setfield(l, -2, "scaleVec2");
  lua_pop(l, 1);
  return 0;
}
