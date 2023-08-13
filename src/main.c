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

#include <p2g/log.h>
#include <p2g/bench.h>
#include <p2g/gs.h>
#include <p2g/pad.h>
#include <p2g/ps2luaprog.h>
#include <p2g/script.h>
#include <p2g/utils.h>

extern int ps2luaprog_is_running;

#ifndef LOG_LEVEL_DEFAUT
#define LOG_LEVEL_DEFAULT LOG_LEVEL_TRACE
#endif

int log_output_level = LOG_LEVEL_DEFAULT;

#define BASE_PATH_MAX_LEN 180
#define FILE_NAME_MAX_LEN 300
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

void core_error(const char *msg) {
  ps2luaprog_is_running = 0;
  logerr("FATAL ERROR: %s", msg);
}

static int ps2lua_scr_print(lua_State *l) {
  const char *msg = lua_tostring(l, 1);
  scr_printf("LUA: %s\n", msg);
  return 0;
}

// Only screen print stuff during startup
#ifndef NO_SCREEN_PRINT
#ifdef info
#undef info
#endif
#define info(m, ...)                                                           \
  printf("[INIT] " m "\n", ##__VA_ARGS__);                                     \
  scr_printf(m "\n", ##__VA_ARGS__)
#endif

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
    luaL_traceback(l, l, err, 0);
    const char *traceback = lua_tostring(l, -1);
    logerr("lua execution error (runfile %s)\n%s", fname, traceback);
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
    base_path[last_sep + 1] = 0;
    trace("got base path = %s", base_path);
  }

  snprintf(init_script, FILE_NAME_MAX_LEN, "%sscript/p2g/init.lua", base_path);
  snprintf(main_script, FILE_NAME_MAX_LEN, "%sscript/main.lua", base_path);

  char *startup = main_script;
  if (argc > 1) {
    info("setting entrypoint to %s", argv[1]);
    startup = argv[1];
  }

  info("default log level = %d", log_output_level);

  BENCH_START(lua_init_time);

  struct lua_State *L;
  L = luaL_newstate();
  if (!L) {
    fatal("failed to startup lua state");
  }
  luaL_openlibs(L);

  ps2luaprog_init(L);

  bind_core_libs(L);
  lua_setglobal(L, "P2GCORE");

  BENCH_INFO(lua_init_time, "lua startup time = %f");
  info("finished lua state setup");

  lua_pushstring(L, base_path);
  lua_setglobal(L, "P2G_ROOT");

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

  if (ps2luaprog_onstart(L)) {
    fatal("failed to run starup callback");
  }
  lua_pushnil(L);
  lua_setglobal(L, "dbgPrint");

  lua_pushinteger(L, 0);
  lua_setglobal(L, "FPS");

  int frame_count = 0;
  clock_t next_fps_report = clock() + CLOCKS_PER_SEC;

  ps2luaprog_is_running = 1;

  while (ps2luaprog_is_running) {
    trace("MAIN - BEGIN FRAME");
    pad_frame_start();
    pad_poll();
    dma_wait_fast();
    if(ps2luaprog_onframe(L)) {
      info("halting program - lua frame exception");
      ps2luaprog_is_running = 0;
    };
    // may be required? -- dma_wait_fast();
    trace("WAIT DRAW");
    // TODO(tommarks): draw2d flag to know if draw was submitted
    draw_wait_finish();
    trace("WAIT VSYNC");
    graph_wait_vsync();
    trace("FLIP");
    gs_flip();
    trace("FLIPOUT");
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
