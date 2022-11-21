#include <lauxlib.h>
#include <lua.h>

#include <stdlib.h>

#include <p2g/log.h>
#include <p2g/script.h>

static const char libname[] = "P2G.slotlist";

#define SLOT_FREE 0

struct slot_list {
  int *slots;
  int *states;
  int capacity;
  int head;
};

static void slot_list_next(struct slot_list *sl) {
  sl->head = (sl->head + 1) % sl->capacity;
}

int slot_list_set(struct slot_list *sl, int i, int m, int state) {
  if (!sl) {
    info("cannot set index of NULL slot list");
    return 0;
  }

  if (i < 0 || i >= sl->capacity) {
    info("cannot set index out of bounds of slot list: capacity = 0 -> %d, "
         "index = %d",
         sl->capacity, i);
    return 0;
  }

  sl->slots[i] = m;
  sl->states[i] = state;
  return 1;
}

int slot_list_push(struct slot_list *sl, int m, int new_state) {
  if (!sl) {
    info("cannot push element into NULL slot list");
    return 0;
  }

  for (int i = 0; i < sl->capacity; i++) {
    if (sl->states[sl->head] == SLOT_FREE) {
      // if slot is free, set this slot and return
      slot_list_set(sl, sl->head, m, new_state);
      slot_list_next(sl);
      return 1;
    } else {
      // otherwise increment the head
      slot_list_next(sl);
    }
  }

  info("no slots left in list");
  return 0;
}

int slot_list_lua_push(lua_State *l) {
  // lua args: 1 = slot_list, 2 = value, 3 = state
  struct slot_list *st = lua_touserdata(l, 1);
  if (!st) {
    luaL_error(l, "cannot push to null slot list");
  }

  lua_pushvalue(l, 2);
  int r = luaL_ref(l, LUA_REGISTRYINDEX);
  int state = lua_tointeger(l, 3);
  if (!slot_list_push(st, r, state)) {
    luaL_error(l, "no slots left in slot list");
    // noreturn
  }
  return 0;
}

int slot_list_lua_each(lua_State *l) {
  // lua args: 1 = slot_list, 2 = fn to call on each slot
  struct slot_list *st = lua_touserdata(l, 1);
  if (!st) {
    luaL_error(l, "cannot call each on null slot list");
  }

  for (int i = 0; i < st->capacity; i++) {
    if (st->states[i] > SLOT_FREE) {
      // push function
      lua_pushvalue(l, 2);
      // push value @ slot
      lua_rawgeti(l, LUA_REGISTRYINDEX, st->slots[i]);
      // push state @ slot
      lua_pushinteger(l, st->states[i]);
      // push index
      lua_pushinteger(l, i);
      // call func with 3 args
      if (lua_pcall(l, 3, 0, 0) != LUA_OK) {
        const char *err = lua_tostring(l, -1);
        luaL_traceback(l, l, err, 0);
        const char *traceback = lua_tostring(l, -1);
        logerr("slot iterator error: index %d:\n%s", i, traceback);
      }
    }
  }
  return 0;
}

int slot_list_lua_each_state(lua_State *l) {
  // lua args: 1 = slot_list, 2 = expected state, 3 = fn to call on each slot
  struct slot_list *st = lua_touserdata(l, 1);
  if (!st) {
    luaL_error(l, "cannot call each-state on null slot list");
  }

  int state = lua_tointeger(l, 2);
  for (int i = 0; i < st->capacity; i++) {
    if (st->states[i] == state) {
      // push function
      lua_pushvalue(l, 3);
      // push value @ slot
      lua_rawgeti(l, LUA_REGISTRYINDEX, st->slots[i]);
      // push state @ slot
      lua_pushinteger(l, st->states[i]);
      // push index
      lua_pushinteger(l, i);
      // call func with 3 args
      if (lua_pcall(l, 3, 0, 0) != LUA_OK) {
        const char *err = lua_tostring(l, -1);
        luaL_traceback(l, l, err, 0);
        const char *traceback = lua_tostring(l, -1);
        logerr("slot iterator error: index %d:\n%s", i, traceback);
      }
    }
  }
  return 0;
}

int slot_list_lua_set_state(lua_State *l) {
  // lua args: 1 = slot_list, 2 = index, 3 = new state
  struct slot_list *st = lua_touserdata(l, 1);
  if (!st) {
    luaL_error(l, "cannot state state of null slot list");
  }

  int index = lua_tointeger(l, 2);
  int new_state = lua_tointeger(l, 3);
  if (index < 0 || index >= st->capacity) {
    luaL_error(l, "index out of range: %d", index);
    // noreturn
  }
  int m = st->slots[index];
  slot_list_set(st, index, 0, new_state);
  if (new_state == SLOT_FREE) {
    luaL_unref(l, LUA_REGISTRYINDEX, m);
  }
  return 0;
}

int slot_list_lua_new(lua_State *l) {
  // lua args: 1 = capacity
  int capacity = lua_tointeger(l, 1);
  trace("allocate new slot list with capacity=%d", capacity);
  struct slot_list *st = lua_newuserdata(l, sizeof(struct slot_list));
  if (!st) {
    luaL_error(l, "failed to allocate new userdata");
    // noreturn
  }

  st->slots = (int *)calloc(capacity, sizeof(int));
  st->states = (int *)calloc(capacity, sizeof(int));
  st->capacity = capacity;
  st->head = 0;

  luaL_getmetatable(l, libname);
  lua_setmetatable(l, -2);

  return 1;
}

int slot_list_lua_free(lua_State *l) {
  trace("cleanup slot list");
  struct slot_list *st = lua_touserdata(l, 1);
  if (!st) {
    luaL_error(l, "failed to cleanup NULL slot list");
    // noreturn
  }

  free(st->slots);
  free(st->states);
  trace("cleanup slot list complete");
  return 0;
}

static const struct luaL_Reg lib[] = {
    {"new", slot_list_lua_new},
    {0, 0},
};

static const struct luaL_Reg methods[] = {
    {"push", slot_list_lua_push},
    {"setState", slot_list_lua_set_state},
    {"each", slot_list_lua_each},
    {"eachState", slot_list_lua_each_state},
    {0, 0},
};

int slot_list_lua_init(lua_State *l) {
  luaL_newmetatable(l, libname);
  lua_pushvalue(l, -1);
  lua_setfield(l, -2, "__index");
  luaL_setfuncs(l, methods, 0);
  lua_pop(l, 1);
  luaL_newlib(l, lib);
  return 1;
}
