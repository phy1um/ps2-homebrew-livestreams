
#ifndef SRC_LUA_H
#define SRC_LUA_H

#include <lua.h>

int gs_lua_init(lua_State *l);
int dma_lua_init(lua_State *l);
int drawlua_init(lua_State *l);
int loglua_init(lua_State *l);
int lua_tga_init(lua_State *l);
int draw2d_lua_init(lua_State *l);

// math...
int vec2lua_init(lua_State *l);
int vec3lua_init(lua_State *l);
int mat3lua_init(lua_State *l);
int floatmath_init(lua_State *l);

#endif
