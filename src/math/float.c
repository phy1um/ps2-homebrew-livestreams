#include <math.h>
#include <lua.h>

#include "log.h"

#define EPSILON 0.000001

static int float_compare(lua_State *l) {
  float a = lua_tonumber(l, 1);
  float b = lua_tonumber(l, 2);
  int res = fabs(a-b) < EPSILON;
  lua_pushboolean(l, res);
  return 1;
}

int floatmath_init(lua_State *l) {
  lua_getglobal(l, "MATH_C_LIB");
  lua_pushcfunction(l, float_compare);
  lua_setfield(l, -2, "floatCmp");
  lua_pop(l, 1);
  return 0;
}
