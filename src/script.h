
#ifndef SRC_LUA_H
#define SRC_LUA_H

#include <lua.h>

struct gs_state {
  framebuffer_t fb;
  zbuffer_t zb;
  unsigned char clear_r;
  unsigned char clear_g;
  unsigned char clear_b;
};



int gs_lua_init(lua_State *l);
int dma_lua_init(lua_State *l);
int drawlua_init(lua_State *l);

#endif
