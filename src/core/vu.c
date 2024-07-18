#include "../draw/draw.h"
#include <p2g/log.h>
#include <lauxlib.h>
#include <lua.h>

static const char * VU_PROG_METATABLE = "ps2.vuprog";
#define PROG_STATE_UNBOUND 0
#define PROG_STATE_BOUND_VU0 1
#define PROG_STATE_BOUND_VU1 2

static int vu1_alloc_head = 0;

static int vu_state(lua_State *l) {
  lua_getfield(l, 1, "_state");
  return 1;
}

static int vu_upload(lua_State *l) {
  int target = lua_tointeger(l, 2);
  if (target != TARGET_VU1) {
    luaL_error(l, "only VU1 supported");
    return 1;
  }
  if (!lua_istable(l, 1)) {
    logerr("vu upload: first argument must be table");
    luaL_error(l, "first argument must be table");
    return 1;
  }
  lua_getfield(l, 1, "buffer");
  if (!lua_istable(l, -1)) {
    int type = lua_type(l, -1);
    const char *type_name = lua_typename(l, type);
    logerr("vu upload: first argument must be a buffer obj");
    luaL_error(l, "first argument must be a buffer object, got: %s", type_name);
    return 1;
  }

  lua_getfield(l, -1, "size");
  int size = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_getfield(l, -1, "ptr");
  void *ptr = lua_touserdata(l, -1);
  lua_pop(l, 1);
  int rc = draw_vu_upload_program(ptr, size, vu1_alloc_head, target);
  if (rc != 0) {
    luaL_error(l, "vu program upload failed");
    return 1;
  }
  lua_pushinteger(l, vu1_alloc_head);
  lua_setfield(l, 1, "address");
  vu1_alloc_head += size;
  while (vu1_alloc_head % 16 != 0) {
    vu1_alloc_head += 1;
  }
  lua_pushinteger(l, PROG_STATE_BOUND_VU1);
  lua_setfield(l, 1, "_state");
  return 0;
}

static int vu_upload_to(lua_State *l) {
  int target = lua_tointeger(l, 2);
  if (target != TARGET_VU1) {
    luaL_error(l, "only VU1 supported");
    return 1;
  }
  if (!lua_istable(l, 1)) {
    logerr("vu upload: first argument must be table");
    luaL_error(l, "first argument must be table");
    return 1;
  }
  lua_getfield(l, 1, "buffer");
  if (!lua_istable(l, -1)) {
    int type = lua_type(l, -1);
    const char *type_name = lua_typename(l, type);
    logerr("vu upload: first argument must be a buffer obj");
    luaL_error(l, "first argument must be a buffer object, got: %s", type_name);
    return 1;
  }

  int addr = lua_tointeger(l, 3);
  trace("lua upload to: vu addr=%d", addr);

  lua_getfield(l, -1, "size");
  int size = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_getfield(l, -1, "ptr");
  void *ptr = lua_touserdata(l, -1);
  lua_pop(l, 1);
  int rc = draw_vu_upload_program(ptr, size, addr, target);
  if (rc != 0) {
    luaL_error(l, "vu program upload failed");
    return 1;
  }
  lua_pushinteger(l, addr);
  lua_setfield(l, 1, "address");
  lua_pushinteger(l, PROG_STATE_BOUND_VU1);
  lua_setfield(l, 1, "_state");
  return 0;
}

static int vu_call(lua_State *l) {
  trace("get state of VU prog");
  lua_getfield(l, 1, "_state");
  int state = lua_tointeger(l, -1);
  lua_pop(l, 1);
  if (state == PROG_STATE_UNBOUND) {
    lua_getfield(l, 1, "name");
    const char *name = lua_tostring(l, -1);
    luaL_error(l, "unbound program call \"%s\"", name);
    return 1;
  }
  trace("get addr of VU prog");
  lua_getfield(l, 1, "address");
  int addr = lua_tointeger(l, -1);
  lua_pop(l, 1);
  trace("calling VU prog");
  int rc = draw_vu_call_program(addr);
  if (rc != 0) {
    lua_getfield(l, 1, "name");
    const char *name = lua_tostring(l, -1);
    luaL_error(l, "call program \"%s\"", name);
    return 1;
  }
  return 0;
}

static int vu_free(lua_State *l) {
  return 0;
}

static int vu_new(lua_State *l) {
  const char *name = lua_tostring(l, 1);
  lua_createtable(l, 0, 4); 
  lua_pushinteger(l, PROG_STATE_UNBOUND);
  lua_setfield(l, -2, "_state");
  lua_pushstring(l, name);
  lua_setfield(l, -2, "name");
  lua_pushinteger(l, 0);
  lua_setfield(l, -2, "address");
  lua_pushvalue(l, 2);
  if (!lua_istable(l, -1)) {
    logerr("(%s): 2nd arg must be a buffer object (table)", name);
    luaL_error(l, "(%s): 2nd arg must be a buffer object (table)", name);
    return 1;
  }
  lua_setfield(l, -2, "buffer");
  luaL_getmetatable(l, VU_PROG_METATABLE);
  lua_setmetatable(l, -2);

  return 1;
}

static int vu_clear(lua_State *l) {
  vu1_alloc_head = 0;
  return 0;
}

int vu_lua_init(lua_State *l) {
  luaL_newmetatable(l, VU_PROG_METATABLE);
  lua_createtable(l, 0, 3);
  lua_pushcfunction(l, vu_state);
  lua_setfield(l, -2, "state");
  lua_pushcfunction(l, vu_upload);
  lua_setfield(l, -2, "upload");
  lua_pushcfunction(l, vu_upload_to);
  lua_setfield(l, -2, "upload_to");
  lua_pushcfunction(l, vu_call);
  lua_setfield(l, -2, "call");
  lua_pushcfunction(l, vu_free);
  lua_setfield(l, -2, "free");

  lua_setfield(l, -2, "__index");
  lua_pop(l, 1);

  lua_createtable(l, 0, 4);
  lua_pushcfunction(l, vu_new);
  lua_setfield(l, -2, "new_program");
  lua_pushcfunction(l, vu_clear);
  lua_setfield(l, -2, "clear_programs");
  lua_pushinteger(l, TARGET_VU0);
  lua_setfield(l, -2, "VU0");
  lua_pushinteger(l, TARGET_VU1);
  lua_setfield(l, -2, "VU1");

  return 1;
}
