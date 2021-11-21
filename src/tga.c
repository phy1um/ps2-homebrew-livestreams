#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

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

char tmp_buffer[256 * 256 * 4];
int load_tga_to_raw(const char *fname, void *buffer) {
  info("loading TGA %s", fname);
  FILE *f = fopen(fname, "rb");
  struct tga_header header = {0};
  size_t rc = fread(&header, 1, 18, f);

  info("reading ID data - %d bytes", header.idlen);
  char idData[255];
  rc = fread(idData, 1, header.idlen, f);

  // bytes per pixel from bits per pixel
  int bpp = header.bps / 8;
  info("reading image data - %d bytes", header.width * header.height * bpp);
  rc = fread(tmp_buffer, bpp, header.width * header.height * bpp, f);
  char *to = (char *)buffer;
  for (int i = 0; i < header.width; i++) {
    for (int j = 0; j < header.height; j++) {
      to[(j * header.width + i) * 4 + 3] =
          tmp_buffer[(j * header.width + i) * 4 + 3];
      to[(j * header.width + i) * 4 + 2] =
          tmp_buffer[(j * header.width + i) * 4 + 0];
      to[(j * header.width + i) * 4 + 1] =
          tmp_buffer[(j * header.width + i) * 4 + 1];
      to[(j * header.width + i) * 4 + 0] =
          tmp_buffer[(j * header.width + i) * 4 + 2];
    }
  }
  fclose(f);
  return 1;
}

int load_tga_lua(lua_State *l) {
  const char *fname = lua_tostring(l, 1);
  int width = lua_tointeger(l, 2);
  int height = lua_tointeger(l, 3);
  uint32_t *b = malloc(width * height * 4);
  load_tga_to_raw(fname, b);

  lua_createtable(l, 0, 5);
  lua_pushinteger(l, 0);
  lua_setfield(l, -2, "head");
  lua_pushinteger(l, width * height * 4);
  lua_setfield(l, -2, "size");
  lua_pushlightuserdata(l, b);
  lua_setfield(l, -2, "ptr");
  lua_pushinteger(l, (int)b);
  lua_setfield(l, -2, "addr");

  luaL_getmetatable(l, "ps2.buffer");
  lua_setmetatable(l, -2);

  // return new buffer
  return 1;
}

int lua_tga_init(lua_State *l) {
  lua_createtable(l, 0, 1);
  lua_pushcfunction(l, load_tga_lua);
  lua_setfield(l, -2, "load");
  lua_setglobal(l, "TGA");
  return 0;
}
