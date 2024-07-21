#include <lua.h>

#include <p2g/log.h>
#include <p2g/ps2math.h>

#define buffer_null_check(p, s)                                                \
  do {                                                                         \
    if (!p) {                                                                  \
      lua_pushstring(l, "got null buffer for " s);                             \
      lua_error(l);                                                            \
      return 1;                                                                \
    }                                                                          \
  } while (0)

#define get_buf(i, to, msg)                                                    \
  lua_pushstring(l, "ptr");                                                    \
  lua_gettable(l, i);                                                          \
  float *to = lua_touserdata(l, -1);                                           \
  buffer_null_check(to, msg " [" #i "]");                                      \
  ((void)0)

static int lua_add_vec4(lua_State *l) {
  get_buf(1, buf1, "copy vec4");
  get_buf(2, buf2, "copy vec4");

  buf1[0] += buf2[0];
  buf1[1] += buf2[1];
  buf1[2] += buf2[2];
  buf1[3] += buf2[3];

  return 0;
}

static int lua_sub_vec4(lua_State *l) {
  get_buf(1, buf1, "sub vec4");
  get_buf(2, buf2, "sub vec4");

  buf1[0] -= buf2[0];
  buf1[1] -= buf2[1];
  buf1[2] -= buf2[2];
  buf1[3] -= buf2[3];

  return 0;
}

// local c = copy(to, from)
static int lua_copy_vec4(lua_State *l) {
  get_buf(1, buf1, "copy vec4");
  get_buf(2, buf2, "copy vec4");

  buf1[0] = buf2[0];
  buf1[1] = buf2[1];
  buf1[2] = buf2[2];
  buf1[3] = buf2[3];

  return 0;
}

static int lua_length_vec4(lua_State *l) {
  get_buf(1, buf1, "length vec4");
  float result = p2m_vec4_length(buf1);
  lua_pushnumber(l, result);
  return 1;
}

static int lua_normalize_vec4(lua_State *l) {
  get_buf(1, buf1, "normalize vec4");
  float len = p2m_vec4_length(buf1);
  buf1[0] /= len;
  buf1[1] /= len;
  buf1[2] /= len;
  buf1[3] /= len;
  return 0;
}

static int lua_dot_vec4(lua_State *l) {
  get_buf(1, buf1, "dot vec4");
  get_buf(2, buf2, "dot vec4");
  float dot = p2m_vec4_dot(buf1, buf2);
  lua_pushnumber(l, dot);
  return 1;
}

static int lua_scale_vec4(lua_State *l) {
  get_buf(1, buf1, "scale vec4");
  float scalar = lua_tonumber(l, 2);
  p2m_vec4_scale(buf1, scalar);
  return 0;
}

int vec4_lua_init(lua_State *l) {
  trace("init core lib math.vec4");
  lua_createtable(l, 0, 8);
  lua_pushcfunction(l, lua_add_vec4);
  lua_setfield(l, -2, "addVec4");
  lua_pushcfunction(l, lua_sub_vec4);
  lua_setfield(l, -2, "subVec4");
  lua_pushcfunction(l, lua_copy_vec4);
  lua_setfield(l, -2, "copyVec4");
  lua_pushcfunction(l, lua_length_vec4);
  lua_setfield(l, -2, "lenVec4");
  lua_pushcfunction(l, lua_normalize_vec4);
  lua_setfield(l, -2, "normalizeVec4");
  lua_pushcfunction(l, lua_dot_vec4);
  lua_setfield(l, -2, "dotVec4");
  lua_pushcfunction(l, lua_scale_vec4);
  lua_setfield(l, -2, "scaleVec4");
  return 1;
}
