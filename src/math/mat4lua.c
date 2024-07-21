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

static int lua_add_mat4(lua_State *l) {
  get_buf(1, buf1, "add m3");
  get_buf(2, buf2, "add m3");

  p2m_m4_add(buf1, buf2);

  return 0;
}

static int lua_copy_mat4(lua_State *l) {
  get_buf(1, buf1, "copy m4");
  get_buf(2, buf2, "copy m4");

  p2m_m4_copy(buf1, buf2);

  return 0;
}

static int lua_identity_mat4(lua_State *l) {
  get_buf(1, buf1, "idetity m4");
  p2m_m4_identity(buf1);
  return 0;
}

static int lua_multiply_mat4(lua_State *l) {
  get_buf(1, buf1, "mul m4");
  get_buf(2, buf2, "mul m4");
  p2m_m4_multiply(buf1, buf2, buf1);
  return 0;
}

static int lua_apply_mat4(lua_State *l) {
  get_buf(1, buf1, "apply m4");
  get_buf(2, buf2, "apply m4 (v4)");
  p2m_m4_apply(buf1, buf2);
  return 0;
}

int mat4_lua_init(lua_State *l) {
  trace("init core lib math.mat4");
  lua_createtable(l, 0, 8);
  lua_pushcfunction(l, lua_add_mat4);
  lua_setfield(l, -2, "addMat4");
  lua_pushcfunction(l, lua_copy_mat4);
  lua_setfield(l, -2, "copyMat4");
  lua_pushcfunction(l, lua_identity_mat4);
  lua_setfield(l, -2, "identityMat4");
  lua_pushcfunction(l, lua_multiply_mat4);
  lua_setfield(l, -2, "mulMat4");
  lua_pushcfunction(l, lua_apply_mat4);
  lua_setfield(l, -2, "applyMat4");
  return 1;
}
