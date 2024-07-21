#include <lua.h>

#include <p2g/log.h>

typedef struct {
  const char *name;
  lua_CFunction open;
} script_binding;

int gs_lua_init(lua_State *l);
int dma_lua_init(lua_State *l);
int draw_lua_init(lua_State *l);
int log_lua_init(lua_State *l);
int tga_lua_init(lua_State *l);
int io_lua_init(lua_State *l);
int draw2d_lua_init(lua_State *l);
int pad_lua_init(lua_State *l);
int slotlist_lua_init(lua_State *l);
int vec2_lua_init(lua_State *l);
int vec3_lua_init(lua_State *l);
int vec4_lua_init(lua_State *l);
int mat3_lua_init(lua_State *l);
int mat4_lua_init(lua_State *l);
int floatmath_lua_init(lua_State *l);


static script_binding SCRIPT_CORE_LIBS[] = {
    {"gs", gs_lua_init},
    {"dma", dma_lua_init},
    {"pad", pad_lua_init},
    {"buffer", draw_lua_init},
    {"log", log_lua_init},
    {"tga", tga_lua_init},
    {"io", io_lua_init},
    {"slotlist", slotlist_lua_init},
    {"draw2d", draw2d_lua_init},
    {"math_vec2", vec2_lua_init},
    {"math_vec3", vec3_lua_init},
    {"math_vec4", vec4_lua_init},
    {"math_mat3", mat3_lua_init},
    {"math_mat4", mat4_lua_init},
    {"math_misc", floatmath_lua_init},
};

int bind_core_libs(lua_State *l) {
  int num_libs = sizeof(SCRIPT_CORE_LIBS) / sizeof(script_binding);
  info("initializing %d core libraries", num_libs);
  lua_createtable(l, 0, num_libs);
  for (int i = 0; i < num_libs; i++) {
    script_binding *b = &SCRIPT_CORE_LIBS[i];
    trace("init core lib %s", b->name);
    if (b->open(l) != 1) {
      logerr("failed to open library: %s", b->name);
      return 0;
    }
    lua_setfield(l, -2, b->name);
    trace("core set field: %s", b->name);
  }
  return 1;
}


