#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <debug.h>
#include <kernel.h>
#include <sifrpc.h>

#include "log.h"

#include "gs.h"
#include "pad.h"
#include "script.h"

static clock_t __time_now;

#define BENCH_START(vname) clock_t vname = clock()
#define BENCH_INFO(vname, m) do { \
  __time_now = clock(); \
  info(m, ((float)__time_now - vname) / (float)CLOCKS_PER_SEC); \
}while(0)
  

#define BASE_PATH_MAX_LEN 60
#define FILE_NAME_MAX_LEN 150
char base_path[BASE_PATH_MAX_LEN] = "host:";
char init_script[FILE_NAME_MAX_LEN];
char main_script[FILE_NAME_MAX_LEN];

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
  BENCH_START(timer);
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "start");
  lua_gettable(l, -2);
  /*
  int type = lua_type(l, -1);
  info("start fn has type :: %s (%d)", lua_typename(l, type), type);
  */

  int rc = lua_pcall(l, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (start event) -- %s", err);
  }
  BENCH_INFO(timer, " PS2PROG onstart in %f");

  return 0;
}

int ps2luaprog_onframe(lua_State *l) {
  lua_getglobal(l, "PS2PROG");
  lua_pushstring(l, "frame");
  lua_gettable(l, -2);
  /*
  int type = lua_type(l, -1);
  info("frame fn has type :: %s (%d)", lua_typename(l, type), type);
  */

  int rc = lua_pcall(l, 0, 0, 0);

  if (rc) {
    const char *err = lua_tostring(l, -1);
    logerr("lua execution error (frame event) -- %s", err);
  }

  return 0;
}

// Only screen print stuff during startup
#ifndef NO_SCREEN_PRINT
#ifdef info
#undef info
#endif
#define info(m, ...)                                                           \
  printf("[INFO] " m "\n", ##__VA_ARGS__);                                     \
  scr_printf(m "\n", ##__VA_ARGS__)
#endif

int ps2luaprog_is_running(lua_State *l) { return 1; }

static int runfile(lua_State *l, const char *fname) {
  BENCH_START(runfile_time);
  info("running lua file %s", fname);
  int rc = luaL_loadfile(l, fname);
  if (rc == LUA_ERRSYNTAX) {
    info("failed to load %s: syntax error", fname);
    const char *err = lua_tostring(l, -1);
    logerr("err: %s", err);
    return -1;
  } else if (rc == LUA_ERRMEM) {
    info("faild to allocate memory for %s", fname);
    return -1;
  } else if (rc == LUA_ERRFILE) {
    info("could not open/read file %s", fname);
    return -1;
  } else if (rc) {
    info("unknown error loading %s", fname);
    return -1;
  }
  BENCH_INFO(runfile_time, " - load time %f");

  BENCH_START(call_time);
  rc = lua_pcall(l, 0, 0, 0);
  if (rc) {
    const char *err = lua_tostring(l, -1);
    info("lua error: %s", err);
    logerr("lua execution error -- %s", err);
    return -1;
  }
  BENCH_INFO(call_time, " - pcall time %f");

  BENCH_INFO(runfile_time, " - total run time %f");

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
    if (last_sep + 2 >= BASE_PATH_MAX_LEN) {
      fatal("base path too long!");
    }
    strncpy(base_path, argv[0], last_sep + 1);
    base_path[last_sep + 2] = 0;
  }

  snprintf(init_script, FILE_NAME_MAX_LEN, "%sscript/ps2init.lua", base_path);
  snprintf(main_script, FILE_NAME_MAX_LEN, "%sscript/main.lua", base_path);

  char *startup = main_script;
  if (argc > 1) {
    info("setting entrypoint to %s", argv[1]);
    startup = argv[1];
  }

  BENCH_START(lua_init_time);

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

  BENCH_INFO(lua_init_time, "lua startup time = %f");
  info("finished lua state setup");

  lua_pushstring(L, base_path);
  lua_setglobal(L, "PS2_SCRIPT_PATH");

  info("binding screen print fn");
  lua_pushcfunction(L, ps2lua_scr_print);
  lua_setglobal(L, "dbgPrint");

  if (runfile(L, init_script)) {
    info("failed to run file %s", init_script);
    fatal("failed to run startup file %s", init_script);
  }
  if (runfile(L, startup)) {
    info("failed to run file %s", startup);
    fatal("failed to run startup file %s", startup);
  }

  ps2luaprog_onstart(L);
  lua_pushnil(L);
  lua_setglobal(L, "dbgPrint");

  lua_pushinteger(L, 0);
  lua_setglobal(L, "FPS");


  int frame_count = 0;
  clock_t next_fps_report = clock() + CLOCKS_PER_SEC;
  while (ps2luaprog_is_running(L)) {
    pad_frame_start();
    pad_poll();
    dma_wait_fast();
    ps2luaprog_onframe(L);
    // may be required? -- dma_wait_fast();
    info("WAIT DRAW");
    draw_wait_finish();
    info("WAIT VSYNC");
    graph_wait_vsync();
    info("FLIP");
    gs_flip();
    info("FLIPOUT");
    frame_count += 1;
    clock_t now = clock();
    if (now > next_fps_report) {
      lua_pushinteger(L, frame_count);
      lua_setglobal(L, "FPS");
      frame_count = 0;
      next_fps_report = now + CLOCKS_PER_SEC;
    }
  }

  info("main loop ended");
}
