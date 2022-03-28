
#ifndef SRC_LUA_H
#define SRC_LUA_H

#include <lua.h>

#define LIB_PREFIX "P2Garage."
#define MAKE_LUA_LIB_NAME(n) (LIB_PREFIX n)

typedef struct {
  const char *name;
  lua_CFunction open;
} script_binding;

int gs_lua_init(lua_State *l);
int dma_lua_init(lua_State *l);
int drawlua_init(lua_State *l);
int loglua_init(lua_State *l);
int lua_tga_init(lua_State *l);
int draw2d_lua_init(lua_State *l);
int pad_lua_init(lua_State *l);

int slot_list_lua_init(lua_State *l);

// math...
int vec2lua_init(lua_State *l);
int vec3lua_init(lua_State *l);
int mat3lua_init(lua_State *l);
int floatmath_init(lua_State *l);

#endif
