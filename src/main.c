#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <math.h>
#include <string.h>

#include <debug.h>
#include <kernel.h>
#include <sifrpc.h>

#include "log.h"

#include "gs.h"
#include "pad.h"
#include "script.h"

char base_path[30] = "host:";
char init_script[150];
char main_script[150];

#ifndef NO_SCREEN_PRINT
#ifdef info
#undef info
#endif
#define info(m, ...)                                                           \
  printf("[INFO] " m "\n", ##__VA_ARGS__);                                     \
  scr_printf(m "\n", ##__VA_ARGS__)
#endif

#ifndef WELCOME_LINE
#define WELCOME_LINE "### PS2 Game Engine Test ###"
#endif

#ifndef AUTHOR
#define AUTHOR "Tom Marks - coding.tommarks.xyz"
#endif

#define fatal(msg, ...)                                                        \
  do {                                                                         \
    logerr(msg, ##__VA_ARGS__);                                                \
    info("program died due to fatal error!");                                  \
    while (1) {                                                                \
    }                                                                          \
  } while (0)

/**
 * get the last index of a character in a string
 */
int last_index_of(const char *str, int str_len, char c) {
  int ind = -1;
  for (int i = 0; i < str_len; i++) {
    if (str[i] == c) {
      ind = i;
    }
  }
  return ind;
}

static int ps2lua_scr_print(lua_State *l) {
  const char *msg = lua_tostring(l, 1);
  scr_printf("LUA: %s\n", msg);
  return 0;
}

int lua_tga_init(lua_State *l);

static int ps2luaprog_start_nil(lua_State *l) {
  info("default start...");
  return 0;
}

static int ps2luaprog_frame_nil(lua_State *l) { return 0; }

static int ps2lua_log2(lua_State *l) {
  int n = lua_tointeger(l, 1);
  float res = log2f(n);
  lua_pushnumber(l, res);
  return 1;
}

int ps2luaprog_init(lua_State *l) {
  lua_createtable(l, 0, 2);
  lua_pushcfunction(l, ps2luaprog_start_nil);
  lua_setfield(l, -2, "start");
  lua_pushcfunction(l, ps2luaprog_frame_nil);
  lua_setfield(l, -2, "frame");
  lua_setglobal(l, "PS2PROG");
  lua_pushcfunction(l, ps2lua_log2);
  lua_setglobal(l, "log2");
  return 0;
}

int ps2luaprog_onstart(lua_State *l) {
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "start");
  lua_gettable(l, -2);
  int type = lua_type(l, -1);
  // info("start fn has type :: %s (%d)", lua_typename(l, type), type);
  int rc = lua_pcall(l, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (start event) -- %s", err);
  }

  return 0;
}

int ps2luaprog_onframe(lua_State *l) {
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "frame");
  lua_gettable(l, -2);
  int type = lua_type(l, -1);
  // info("frame fn has type :: %s (%d)", lua_typename(l, type), type);
  //
  int rc = lua_pcall(l, 0, 0, 0);

  if (rc) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (frame event) -- %s", err);
  }

  return 0;
}

int ps2luaprog_is_running(lua_State *l) { return 1; }

static int runfile(lua_State *l, const char *fname) {
  info("running lua file %s", fname);
  int rc = luaL_loadfile(l, fname);
  if (rc == LUA_ERRSYNTAX) {
    logerr("failed to load %s: syntax error", fname);
    const char *err = lua_tostring(l, -1);
    logerr("err: %s", err);
    return -1;
  } else if (rc == LUA_ERRMEM) {
    logerr("faild to allocate memory for %s", fname);
    return -1;
  } else if (rc == LUA_ERRFILE) {
    logerr("could not open/read file %s", fname);
    return -1;
  } else if (rc) {
    logerr("unknown error loading %s", fname);
    return -1;
  }

  rc = lua_pcall(l, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error -- %s", err);
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {

#ifndef NO_SCREEN_PRINT
  init_scr();
  scr_printf("==========\n" WELCOME_LINE "\nBy " AUTHOR "\n==========\n\n");
#endif

  gs_init();

  info("startup - argc = %d", argc);
  for (int i = 0; i < argc; i++) {
    info("arg %d) %s", i, argv[i]);
  }
  if (argc != 0) {
    int len = strlen(argv[0]);
    int last_sep = last_index_of(argv[0], len, '/');
    if (last_sep == -1) {
      last_sep = last_index_of(argv[0], len, '\\');
    }
    if (last_sep == -1) {
      last_sep = last_index_of(argv[0], len, ':');
    }
    if (last_sep == -1) {
      logerr("invalid ELF path in argv[0]: %s", argv[0]);
    }
    strncpy(base_path, argv[0], last_sep);
    base_path[last_sep+1] = 0;
  }

  sprintf(init_script, "%sscript/ps2init.lua", base_path);
  sprintf(main_script, "%sscript/main.lua", base_path);

  char *startup = main_script;
  if (argc > 1) {
    info("setting entrypoint to %s", argv[1]);
    startup = argv[1];
  }

  struct lua_State *L;
  L = luaL_newstate();
  if (!L) {
    fatal("failed to startup lua state");
  }
  luaL_openlibs(L);

  ps2luaprog_init(L);
  dma_lua_init(L);
  gs_lua_init(L);
  lua_tga_init(L);
  pad_lua_init(L);

  // TODO(Tom Marks): better abstraction for drawlua_*
  drawlua_init(L);

  info("finished lua state setup");

  lua_pushstring(L, base_path);
  lua_setglobal(L, "PS2_SCRIPT_PATH");

  info("binding screen print fn");
  lua_pushcfunction(L, ps2lua_scr_print);
  lua_setglobal(L, "dbgPrint");

  if (runfile(L, init_script)) {
    info("failed to load file %s", init_script);
    fatal("failed to load startup file %s", init_script);
  }
  if (runfile(L, startup)) {
    info("failed to load file %s", startup);
    fatal("failed to load startup file %s", startup);
  }

  ps2luaprog_onstart(L);
  lua_pushnil(L);
  lua_setglobal(L, "dbgPrint");

  while (ps2luaprog_is_running(L)) {
    pad_frame_start();
    pad_poll();
    dma_wait_fast();
    ps2luaprog_onframe(L);
    // may be required? -- dma_wait_fast();
    trace("WAIT DRAW");
    draw_wait_finish();
    trace("WAIT VSYNC");
    graph_wait_vsync();
    trace("FLIP");
    gs_flip();
    trace("FLIPOUT");
  }

  info("main loop ended");
}
