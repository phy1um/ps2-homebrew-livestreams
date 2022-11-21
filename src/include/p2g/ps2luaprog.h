
#ifndef PS2LUAPROG_H
#define PS2LUAPROG_H

#include <lua.h>

int ps2luaprog_init(lua_State *l);

int ps2luaprog_onframe(lua_State *l);
int ps2luaprog_onstart(lua_State *l);

#endif
