#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <p2g/log.h>
#include <p2g/utils.h>

struct __attribute__((__packed__)) tga_header {
  uint8_t idlen;
  uint8_t colMapType;
  uint8_t imgType;

  uint16_t firstColEntryIndex;
  uint16_t colMapLength;
  uint8_t colMapBps;

  uint16_t xorigin;
  uint16_t yorigin;
  uint16_t width;
  uint16_t height;
  uint8_t bps;
  uint8_t descriptor;
};

// TGA.header_get(buf, field_name)
static int get_tga_header_from_buffer(lua_State *l) {
  lua_getfield(l, 1, "ptr");
  struct tga_header *header = (struct tga_header*)lua_touserdata(l, -1);
  const char *field = lua_tostring(l, 2);
  if (strcmp(field, "id_length") == 0) {
    lua_pushnumber(l, header->idlen);
    return 1;
  } else if (strcmp(field, "x_origin") == 0) {
    lua_pushnumber(l, header->xorigin);
    return 1;
  } else if (strcmp(field, "y_origin") == 0) {
    lua_pushnumber(l, header->yorigin);
    return 1;
  } else if (strcmp(field, "width") == 0) {
    lua_pushnumber(l, header->width);
    return 1;
  } else if (strcmp(field, "height") == 0) {
    lua_pushnumber(l, header->height);
    return 1;
  } else if (strcmp(field, "bps") == 0) {
    lua_pushnumber(l, header->bps);
    return 1;
  }

  return luaL_error(l, "unknown field \"%s\"", field);
}

// TGA.swizzle16(img)
static int swizzle16(lua_State *l) {
  const int bpp = 2;
  lua_getfield(l, 1, "width");
  int width = lua_tointeger(l, -1);
  lua_getfield(l, 1, "height");
  int height = lua_tointeger(l, -1);
  lua_getfield(l, 1, "data");
  lua_getfield(l, -1, "ptr");
  char *buffer = lua_touserdata(l, -1);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      unsigned char tmp = buffer[(j * width + i) * bpp + 1];
      buffer[(j * width + i) * bpp + 1] =
          buffer[(j * width + i) * bpp];
      buffer[(j * width + i) * bpp] = tmp;
    }
  }
  return 0;
}

static int swizzle24(lua_State *l) {
  const int bpp = 3;
  lua_getfield(l, 1, "width");
  int width = lua_tointeger(l, -1);
  lua_getfield(l, 1, "height");
  int height = lua_tointeger(l, -1);
  lua_getfield(l, 1, "data");
  lua_getfield(l, -1, "ptr");
  char *buffer = lua_touserdata(l, -1);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      unsigned char tmp = buffer[(j * width + i) * bpp + 2];
      buffer[(j * width + i) * bpp + 2] =
          buffer[(j * width + i) * bpp];
      buffer[(j * width + i) * bpp] = tmp;
    }
  }
  return 0;
}

static int swizzle32(lua_State *l) {
  const int bpp = 4;
  lua_getfield(l, 1, "width");
  int width = lua_tointeger(l, -1);
  lua_getfield(l, 1, "height");
  int height = lua_tointeger(l, -1);
  lua_getfield(l, 1, "data");
  lua_getfield(l, -1, "ptr");
  unsigned char *buffer = lua_touserdata(l, -1);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      unsigned char tmp = buffer[(j * width + i) * bpp + 2];
      buffer[((j * width + i) * bpp) + 2] =
          buffer[(j * width + i) * bpp];
      buffer[(j * width + i) * bpp] = tmp;
      // PS2 alpha maps [0,0x80] to TGA's [0,0xFF]
      buffer[((j * width + i) * bpp) + 3] /= 2;
    }
  }
  return 0;
}

int tga_lua_init(lua_State *l) {
  lua_createtable(l, 0, 5);
  lua_pushcfunction(l, get_tga_header_from_buffer);
  lua_setfield(l, -2, "get_header_field");
  lua_pushcfunction(l, swizzle16);
  lua_setfield(l, -2, "swizzle16");
  lua_pushcfunction(l, swizzle24);
  lua_setfield(l, -2, "swizzle24");
  lua_pushcfunction(l, swizzle32);
  lua_setfield(l, -2, "swizzle32");
  lua_pushnumber(l, sizeof(struct tga_header));
  lua_setfield(l, -2, "HEADER_SIZE");
  return 1;
}
