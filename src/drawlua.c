#include <lua.h>

#include "draw/draw.h"
#include "log.h"

int draw2d_lua_frame_start(lua_State *l) {
  draw2d_frame_start(); 
  return 0;
}

int draw2d_lua_frame_end(lua_State *l) {
  draw2d_frame_end();
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

int draw2d_lua_undefined(lua_State *l) {
  logerr("undefined function called");
  return 0;
}

#define pushfn(f, n) \
  lua_pushcfunction(l, f); \
  lua_setfield(l, -2, n)

int draw2d_lua_init(lua_State *l) {
  draw2d_alloc();
  lua_createtable(l, 0, 5);
  pushfn(draw2d_lua_frame_start, "frameStart");
  pushfn(draw2d_lua_frame_end, "frameEnd");
  pushfn(draw2d_lua_triangle, "triangle");
  pushfn(draw2d_lua_undefined, "sprite");
  pushfn(draw2d_lua_set_colour, "setColour");
  pushfn(draw2d_lua_clear_colour, "clearColour");
  pushfn(draw2d_lua_screen_dimensions, "screenDimensions");
  lua_setglobal(l, "FAST_DRAW2D"); 
  return 0;
}
