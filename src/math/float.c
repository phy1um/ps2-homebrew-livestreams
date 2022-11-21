#include <lua.h>
#include <math.h>

#include <p2g/log.h>


#define EPSILON 0.000001

static int float_compare(lua_State *l) {
  float a = lua_tonumber(l, 1);
  float b = lua_tonumber(l, 2);
  int res = fabs(a - b) < EPSILON;
  lua_pushboolean(l, res);
  return 1;
}

int floatmath_lua_init(lua_State *l) {
  trace("init core lib math.misc");
  lua_createtable(l, 0, 1);
  lua_pushcfunction(l, float_compare);
  lua_setfield(l, -2, "floatCmp");
  return 1;
}
