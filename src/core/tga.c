#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

// PRECONDITION: buffer is large enough to hold entire texture
int load_tga_to_raw(const char *fname, unsigned char *buffer, int buffer_len) {
  info("loading TGA %s", fname);
  FILE *f = fopen(fname, "rb");
  if (!f) {
    logerr("failed to read file %s", fname);
    fclose(f);
    return 0;
  }
  struct tga_header header = {0};
  size_t rc = fread(&header, 1, 18, f);
  if (rc != 18) {
    if (feof(f)) {
      logerr("malformed TGA %s, unexpected EOF in header", fname);
      goto ERR;
    } else if (ferror(f)) {
      logerr("error reading %s", fname);
      goto ERR;
    } else {
      logerr("unknown IO error with %s", fname);
      goto ERR;
    }
  }

  info("reading ID data - %d bytes", header.idlen);
  char idData[255];
  rc = fread(idData, 1, header.idlen, f);

  if (header.bps != 4 && header.bps != 8 && header.bps != 16 &&
      header.bps != 24 && header.bps != 32) {
    logerr("unexpected bits per pixel in TGA: %d", header.bps);
    goto ERR;
  }

  // bytes per pixel from bits per pixel
  int bpp = header.bps / 8;
  size_t size = header.width * header.height * bpp;
  if (header.bps == 4) {
    size = header.width * header.height * 0.5;
  }

  if (size > buffer_len) {
    logerr("TGA image data (%d) too large for buffer (%d)", size, buffer_len);
    goto ERR;
  }
  info("reading image data - %d bytes (%d bytes/%d bits)", size, bpp,
       header.bps);
  rc = fread(buffer, 1, size, f);
  if (header.bps == 16) {
    for (int i = 0; i < header.width; i++) {
      for (int j = 0; j < header.height; j++) {
        unsigned char tmp = buffer[(j * header.width + i) * bpp + 1];
        buffer[(j * header.width + i) * bpp + 1] =
            buffer[(j * header.width + i) * bpp];
        buffer[(j * header.width + i) * bpp] = tmp;
      }
    }
  } else if (header.bps == 24 || header.bps == 32) {
    for (int i = 0; i < header.width; i++) {
      for (int j = 0; j < header.height; j++) {
        unsigned char tmp = buffer[(j * header.width + i) * bpp + 2];
        buffer[(j * header.width + i) * bpp + 2] =
            buffer[(j * header.width + i) * bpp];
        buffer[(j * header.width + i) * bpp] = tmp;
        if (header.bps == 32) {
          buffer[(j * header.width + i) * bpp + 3] /= 2;
        }
      }
    }
  }
  fclose(f);
  return 0;
ERR:
  fclose(f);
  return 1;
}

int load_tga_lua(lua_State *l) {
  // ARG1 == fname, ARG2 == buffer
  const char *fname = lua_tostring(l, 1);
  lua_pushstring(l, "ptr");
  lua_gettable(l, 2);
  void *buffer = lua_touserdata(l, -1);

  lua_pushstring(l, "size");
  lua_gettable(l, 2);
  int size = lua_tointeger(l, -1);

  lua_pushstring(l, "head");
  lua_gettable(l, 2);
  int head = lua_tointeger(l, -1);

  int rv = load_tga_to_raw(fname, buffer, size - head);
  if (rv) {
    char msg[200];
    snprintf(msg, sizeof(msg), "failed to load texture %s", fname);
    lua_pushstring(l, msg);
    lua_error(l);
    return 1;
  }

  return 0;
}

#define setint(name, value)                                                    \
  lua_pushinteger(l, value);                                                   \
  lua_setfield(l, -2, name)

int lua_tga_get_header(lua_State *l) {
  const char *fname = lua_tostring(l, 1);

  info("loading TGA header %s", fname);
  FILE *f = fopen(fname, "rb");
  if (!f) {
    logerr("failed to read file %s", fname);
    goto ERR;
  }
  struct tga_header header = {0};
  size_t rc = fread(&header, 1, 18, f);
  if (rc != 18) {
    if (feof(f)) {
      logerr("malformed TGA %s, unexpected EOF in header", fname);
      goto ERR;
    } else if (ferror(f)) {
      logerr("error reading %s", fname);
      goto ERR;
    } else {
      logerr("unknown IO error with %s", fname);
      goto ERR;
    }
  }
  fclose(f);

  lua_createtable(l, 0, 4);
  setint("width", header.width);
  setint("height", header.height);
  setint("bps", header.bps);
  setint("imageType", header.imgType);
  return 1;
ERR:
  fclose(f);
  lua_pushstring(l, "failed to load TGA header");
  lua_error(l);
  return 1;
}

int lua_tga_init(lua_State *l) {
  lua_createtable(l, 0, 1);
  lua_pushcfunction(l, load_tga_lua);
  lua_setfield(l, -2, "load");
  lua_pushcfunction(l, lua_tga_get_header);
  lua_setfield(l, -2, "header");
  return 1;
}
