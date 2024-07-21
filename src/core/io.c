#include <lauxlib.h>
#include <lua.h>

#include <stdio.h>

// io.read_file(name, offset, size, target_buffer)
static int io_read_file(lua_State *l) {
  const char *file_name = lua_tostring(l, 1);
  int read_offset = lua_tointeger(l, 2);
  int read_size = lua_tointeger(l, 3);
  lua_getfield(l, 4, "ptr");
  void *ptr = lua_touserdata(l, -1);
  lua_getfield(l, 4, "size");
  int buffer_size = lua_tointeger(l, -1);
  FILE *f = fopen(file_name, "rb");
  if (!f) {
    return luaL_error(l, "open %s", file_name);
  }

  if (read_size > buffer_size) {
    fclose(f);
    return luaL_error(l, "buffer too small: got %d need %d", buffer_size,
                      read_size);
  }
  fseek(f, read_offset, SEEK_SET);
  size_t rc = fread(ptr, 1, read_size, f);
  if (rc != read_size) {
    fclose(f);
    return luaL_error(l, "read %s", file_name);
  }

  fclose(f);
  return 0;
}

// io.file_size(name)
static int io_file_size(lua_State *l) {
  const char *file_name = lua_tostring(l, 1);
  FILE *f = fopen(file_name, "rb");
  if (!f) {
    return luaL_error(l, "open %s", file_name);
  }

  fseek(f, 0L, SEEK_END);
  size_t file_size = ftell(f);
  fseek(f, 0L, SEEK_SET);

  lua_pushinteger(l, file_size);
  return 1;
}

int io_lua_init(lua_State *l) {
  lua_createtable(l, 0, 2);
  lua_pushcfunction(l, io_file_size);
  lua_setfield(l, -2, "file_size");
  lua_pushcfunction(l, io_read_file);
  lua_setfield(l, -2, "read_file");
  return 1;
}
