#include <lauxlib.h>
#include <lua.h>

#include "../draw/vu.h"

#define bind(b, n)                                                             \
  lua_pushinteger(l, b);                                                       \
  lua_setfield(l, -2, n)

int vif_lua_init(lua_State *l) {
  lua_createtable(l, 0, 3);
  bind(VIF_CODE_UNPACK_V432, "UNPACK_V432");
  bind(TARGET_VU0, "VIF0");
  bind(TARGET_VU1, "VIF1");
  return 1;
}
