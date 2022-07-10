#include <lua.h>

#include "../log.h"
#include "../ps2math.h"

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

static int lua_add_mat3(lua_State *l) {
  get_buf(1, buf1, "add m3");
  get_buf(2, buf2, "add m3");

  p2m_m3_add(buf1, buf2);

  return 0;
}

// local c = copy(to, from)
static int lua_copy_mat3(lua_State *l) {
  get_buf(1, buf1, "copy m3");
  get_buf(2, buf2, "copy m3");

  p2m_m3_copy(buf1, buf2);

  return 0;
}

static int lua_identity_mat3(lua_State *l) {
  get_buf(1, buf1, "idetity m3");
  p2m_m3_identity(buf1);
  return 0;
}

static int lua_multiply_mat3(lua_State *l) {
  get_buf(1, buf1, "mul m3");
  get_buf(2, buf2, "mul m3");
  p2m_m3_multiply(buf1, buf2, buf1);
  return 0;
}

static int lua_apply_mat3(lua_State *l) {
  get_buf(1, buf1, "apply m3");
  get_buf(2, buf2, "apply m3 (v3)");
  p2m_m3_apply(buf1, buf2);
  return 0;
}

int mat3lua_init(lua_State *l) {
  trace("init core lib math.mat3");
  lua_createtable(l, 0, 8);
  lua_pushcfunction(l, lua_add_mat3);
  lua_setfield(l, -2, "addMat3");
  lua_pushcfunction(l, lua_copy_mat3);
  lua_setfield(l, -2, "copyMat3");
  lua_pushcfunction(l, lua_identity_mat3);
  lua_setfield(l, -2, "identityMat3");
  lua_pushcfunction(l, lua_multiply_mat3);
  lua_setfield(l, -2, "mulMat3");
  lua_pushcfunction(l, lua_apply_mat3);
  lua_setfield(l, -2, "applyMat3");
  return 1;
}
