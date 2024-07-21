#include <lua.h>
#include <lauxlib.h>

#include "../draw/draw.h"
#include <p2g/log.h>

int draw2d_lua_frame_start(lua_State *l) {
  draw_frame_start();
  return 0;
}

int draw2d_lua_frame_end(lua_State *l) {
  draw_frame_end();
  return 0;
}

int draw2d_lua_triangle(lua_State *l) {
  float x1 = lua_tonumber(l, 2);
  float y1 = lua_tonumber(l, 3);
  float x2 = lua_tonumber(l, 4);
  float y2 = lua_tonumber(l, 5);
  float x3 = lua_tonumber(l, 6);
  float y3 = lua_tonumber(l, 7);
  draw2d_triangle(x1, y1, x2, y2, x3, y3);
  return 0;
}

int draw2d_lua_textri(lua_State *l) {
  lua_pushstring(l, "width");
  lua_gettable(l, 2);
  int width = lua_tointeger(l, -1);

  lua_pushstring(l, "height");
  lua_gettable(l, 2);
  int height = lua_tointeger(l, -1);

  lua_pushstring(l, "format");
  lua_gettable(l, 2);
  int psm = lua_tointeger(l, -1);

  lua_pushstring(l, "basePtr");
  lua_gettable(l, 2);
  int vram_addr = lua_tointeger(l, -1);

  draw2d_bind_texture(vram_addr, width, height, psm);

  float x1 = lua_tonumber(l, 3);
  float y1 = lua_tonumber(l, 4);
  float u1 = lua_tonumber(l, 5);
  float v1 = lua_tonumber(l, 6);
  float x2 = lua_tonumber(l, 7);
  float y2 = lua_tonumber(l, 8);
  float u2 = lua_tonumber(l, 9);
  float v2 = lua_tonumber(l, 10);
  float x3 = lua_tonumber(l, 11);
  float y3 = lua_tonumber(l, 12);
  float u3 = lua_tonumber(l, 13);
  float v3 = lua_tonumber(l, 14);

  draw2d_textri(x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3);
  return 0;
}

int draw2d_lua_rect(lua_State *l) {
  float x1 = lua_tonumber(l, 2);
  float y1 = lua_tonumber(l, 3);
  float w = lua_tonumber(l, 4);
  float h = lua_tonumber(l, 5);
  draw2d_rect(x1, y1, w, h);
  return 0;
}

int draw2d_lua_screen_dimensions(lua_State *l) {
  int width = lua_tointeger(l, 2);
  int height = lua_tointeger(l, 3);
  draw2d_screen_dimensions(width, height);
  return 0;
}

int draw2d_lua_clear_colour(lua_State *l) {
  int r = lua_tointeger(l, 2);
  int g = lua_tointeger(l, 3);
  int b = lua_tointeger(l, 4);
  draw2d_clear_colour(r, g, b);
  return 0;
}

int draw2d_lua_set_colour(lua_State *l) {
  int r = lua_tointeger(l, 2);
  int g = lua_tointeger(l, 3);
  int b = lua_tointeger(l, 4);
  int a = lua_tointeger(l, 5);
  draw2d_set_colour(r, g, b, a);
  return 0;
}

int draw2d_lua_upload_texture(lua_State *l) {
  lua_pushstring(l, "width");
  lua_gettable(l, 2);
  int width = lua_tointeger(l, -1);
  lua_pop(l, 1);

  lua_pushstring(l, "height");
  lua_gettable(l, 2);
  int height = lua_tointeger(l, -1);
  lua_pop(l, 1);

  lua_pushstring(l, "format");
  lua_gettable(l, 2);
  int psm = lua_tointeger(l, -1);
  lua_pop(l, 1);

  lua_pushstring(l, "basePtr");
  lua_gettable(l, 2);
  int vram_addr = lua_tointeger(l, -1);
  lua_pop(l, 1);

  lua_pushstring(l, "data");
  lua_gettable(l, 2);
  // texture data table on top of stack!
  lua_pushstring(l, "addr");
  lua_gettable(l, -2);
  void *ptr = (void *)lua_tointeger(l, -1);
  lua_pop(l, 2);

  // ASSUME: no textures at VRAM addr 0
  if (vram_addr == 0) {
    logerr("cannot upload texture at vram addr = 0");
    lua_pushboolean(l, 0);
    return 1;
  }

  trace("calling upload texture with: w=%d, h=%d, psm=%d, vram=%d, ee_addr=%p",
       width, height, psm, vram_addr, ptr);
  int rc = draw_upload_texture(ptr, width * height * 4, width, height, psm,
                                 vram_addr);
  lua_pushboolean(l, rc);
  return 1;
}

int draw2d_lua_sprite(lua_State *l) {
  lua_pushstring(l, "width");
  lua_gettable(l, 2);
  int width = lua_tointeger(l, -1);

  lua_pushstring(l, "height");
  lua_gettable(l, 2);
  int height = lua_tointeger(l, -1);

  lua_pushstring(l, "format");
  lua_gettable(l, 2);
  int psm = lua_tointeger(l, -1);

  lua_pushstring(l, "basePtr");
  lua_gettable(l, 2);
  int vram_addr = lua_tointeger(l, -1);

  draw2d_bind_texture(vram_addr, width, height, psm);

  float x = lua_tonumber(l, 3);
  float y = lua_tonumber(l, 4);
  float w = lua_tonumber(l, 5);
  float h = lua_tonumber(l, 6);
  float u1 = lua_tonumber(l, 7);
  float v1 = lua_tonumber(l, 8);
  float u2 = lua_tonumber(l, 9);
  float v2 = lua_tonumber(l, 10);

  draw2d_sprite(x, y, w, h, u1, v1, u2, v2);

  return 1;
}

int draw2d_lua_setclut(lua_State *l) {
  lua_pushstring(l, "basePtr");
  lua_gettable(l, 2);
  int addr = lua_tointeger(l, -1);

  draw2d_set_clut_state(addr);
  return 0;
}

int draw2d_lua_undefined(lua_State *l) {
  logerr("undefined function called");
  return 0;
}

static int draw2d_lua_bind_buffer(lua_State *l) {
  lua_pushstring(l, "ptr");
  lua_gettable(l, 2);
  void *ptr = lua_touserdata(l, -1);
  lua_pushstring(l, "size");
  lua_gettable(l, 2);
  int len = lua_tointeger(l, -1);

  draw_bind_buffer(ptr, len);
  return 0;
}

// self:mesh_cnt(buffer, vert_count, vert_size)
static int draw_lua_mesh_cnt(lua_State *l) {
  lua_pushstring(l, "ptr");
  lua_gettable(l, 2);
  void *ptr = lua_touserdata(l, -1);
  int vertex_count = lua_tointeger(l, 3);
  int vertex_size = lua_tointeger(l, 4);
  size_t out = draw3d_mesh_triangles_cnt(ptr, vertex_count, vertex_size);
  lua_pushinteger(l, out);
  return 1;
}

static int draw_lua_mesh_ref(lua_State *l) {
  luaL_error(l, "unimplemented");
  return 0;
}

// self:ee_transform(matrix, offset, vert_count, vert_size, xyz_offset)
static int draw_lua_ee_transform(lua_State *l) {
  lua_pushstring(l, "ptr");
  lua_gettable(l, 2);
  void *matrix = lua_touserdata(l, -1);
  if (!matrix) {
    luaL_error(l, "matrix is NULL");
    return 1;
  }
  int offset = lua_tointeger(l, 3);
  int vertex_count = lua_tointeger(l, 4);
  int vertex_size = lua_tointeger(l, 5);
  int xyz_offset = lua_tointeger(l, 6);
  int out = draw3d_ee_transform_verts(matrix, offset, vertex_count, vertex_size, xyz_offset);
  lua_pushinteger(l, out);
  return 1;
}

#define pushfn(f, n)                                                           \
  lua_pushcfunction(l, f);                                                     \
  lua_setfield(l, -2, n)

int draw2d_lua_init(lua_State *l) {
  lua_createtable(l, 0, 5);
  pushfn(draw2d_lua_frame_start, "frameStart");
  pushfn(draw2d_lua_frame_end, "frameEnd");
  pushfn(draw2d_lua_triangle, "triangle");
  pushfn(draw2d_lua_textri, "textri");
  pushfn(draw2d_lua_rect, "rect");
  pushfn(draw2d_lua_sprite, "sprite");
  pushfn(draw2d_lua_set_colour, "setColour");
  pushfn(draw2d_lua_clear_colour, "clearColour");
  pushfn(draw2d_lua_screen_dimensions, "screenDimensions");
  pushfn(draw2d_lua_upload_texture, "uploadTexture");
  pushfn(draw2d_lua_setclut, "setClut");
  pushfn(draw2d_lua_bind_buffer, "bindBuffer");
  pushfn(draw_lua_mesh_cnt, "mesh_cnt");
  pushfn(draw_lua_mesh_ref, "mesh_ref");
  pushfn(draw_lua_ee_transform, "ee_transform");
  return 1;
}

