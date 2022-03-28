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


static int lua_add_vec3(lua_State *l) {
  get_buf(1, buf1, "copy vec3");
  get_buf(2, buf2, "copy vec3");

  buf1[0] += buf2[0];
  buf1[1] += buf2[1];
  buf1[2] += buf2[2];

  return 0;
}

static int lua_sub_vec3(lua_State *l) {
  get_buf(1, buf1, "sub vec3");
  get_buf(2, buf2, "sub vec3");

  buf1[0] -= buf2[0];
  buf1[1] -= buf2[1];
  buf1[2] -= buf2[2];

  return 0;
}

// local c = copy(to, from)
static int lua_copy_vec3(lua_State *l) {
  get_buf(1, buf1, "copy vec3");
  get_buf(2, buf2, "copy vec3");

  buf1[0] = buf2[0];
  buf1[1] = buf2[1];
  buf1[2] = buf2[2];

  return 0;
}

static int lua_length_vec3(lua_State *l) {
  get_buf(1, buf1, "length vec3");
  float result = p2m_vec3_length(buf1);
  lua_pushnumber(l, result);
  return 1;
}

static int lua_normalize_vec3(lua_State *l) {
  get_buf(1, buf1, "normalize vec3");
  float len = p2m_vec3_length(buf1);
  buf1[0] /= len;
  buf1[1] /= len;
  buf1[2] /= len;
  return 0;
}

static int lua_dot_vec3(lua_State *l) {
  get_buf(1, buf1, "dot vec3");
  get_buf(2, buf2, "dot vec3");
  float dot = p2m_vec3_dot(buf1, buf2);
  lua_pushnumber(l, dot);
  return 1;
}

static int lua_scale_vec3(lua_State *l) {
  get_buf(1, buf1, "scale vec3");
  float scalar = lua_tonumber(l, 2);
  p2m_vec3_scale(buf1, scalar);
  return 0;
}

int vec3lua_init(lua_State *l) {
  trace("init core lib math.vec3");
  lua_createtable(l, 0, 8);
  lua_pushcfunction(l, lua_add_vec3);
  lua_setfield(l, -2, "addVec3");
  lua_pushcfunction(l, lua_sub_vec3);
  lua_setfield(l, -2, "subVec3");
  lua_pushcfunction(l, lua_copy_vec3);
  lua_setfield(l, -2, "copyVec3");
  lua_pushcfunction(l, lua_length_vec3);
  lua_setfield(l, -2, "lenVec3");
  lua_pushcfunction(l, lua_normalize_vec3);
  lua_setfield(l, -2, "normalizeVec3");
  lua_pushcfunction(l, lua_dot_vec3);
  lua_setfield(l, -2, "dotVec3");
  lua_pushcfunction(l, lua_scale_vec3);
  lua_setfield(l, -2, "scaleVec3");
  return 1;
}
