#include <lua.h>

#include <kernel.h>
#include <libpad.h>
#include <loadfile.h>

#include <malloc.h>

#include "log.h"
#include "pad.h"

#define pad_test(b, v)                                                         \
  do {                                                                         \
    if ((c & b)) {                                                             \
      btn_held[v] = 1;                                                         \
    }                                                                          \
  } while (0)

char *pad_buffer;
struct padButtonStatus *pad_read_space;

int *btn_held;

unsigned char joysticks[JOY_AXIS_COUNT];

int prev_inputs;

int button_held(int b) { return btn_held[b]; }

unsigned char joy_axis_value(int a) { return joysticks[a]; }

int pad_init() {
  btn_held = memalign(256, 12 * sizeof(int));
  pad_buffer = memalign(256, 256);
  if ((u32)pad_buffer & 0xf) {
    info("pad buffer was not 16byte aligned: %x", (int)pad_buffer);
    return -1;
  }
  pad_read_space = memalign(128, sizeof(struct padButtonStatus));

  info("loading SIF modules for PAD");
  int rc = SifLoadModule(R_SIO2MAN, 0, 0);
  if (!rc) {
    logerr("failed to load SIF %s", R_SIO2MAN);
    return 0;
  }
  rc = SifLoadModule(R_PADMAN, 0, 0);
  if (!rc) {
    logerr("failed to load SIF %s", R_PADMAN);
    return 0;
  }
  padInit(0);
  padPortOpen(0, 0, pad_buffer);
  // TODO(phy1um): check for dualshock controller
  padSetMainMode(0, 0, 1, 3);
  return 1;
}

static int pad_wait(int port, int slot, int tries) {
  int now;
  now = padGetState(port, slot);
  if (now == PAD_STATE_DISCONN) {
    return -1;
  }
  while ((now != PAD_STATE_STABLE) && (now != PAD_STATE_FINDCTP1)) {
    now = padGetState(port, slot);
    tries--;
    if (tries == 0) {
      break;
    }
  }
  return 0;
}

void pad_poll() {
  if (pad_wait(0, 0, 10) < 0) {
    return;
  }

  if (padRead(0, 0, pad_read_space) != 0) {
    int pad = 0xffff ^ pad_read_space->btns;
    int c = pad ^ prev_inputs;
    pad_test(PAD_LEFT, DPAD_LEFT);
    pad_test(PAD_RIGHT, DPAD_RIGHT);
    pad_test(PAD_UP, DPAD_UP);
    pad_test(PAD_DOWN, DPAD_DOWN);
    pad_test(PAD_CROSS, BUTTON_X);
    pad_test(PAD_SQUARE, BUTTON_SQUARE);
    pad_test(PAD_TRIANGLE, BUTTON_TRIANGLE);
    pad_test(PAD_CIRCLE, BUTTON_CIRCLE);
    pad_test(PAD_L1, BUTTON_L1);
    pad_test(PAD_L2, BUTTON_L2);
    pad_test(PAD_R1, BUTTON_R1);
    pad_test(PAD_R2, BUTTON_R2);
    /*
    joysticks[AXIS_LEFT_X] = pad_read_space->ljoy_h;
    joysticks[AXIS_LEFT_Y] = pad_read_space->ljoy_v;
    joysticks[AXIS_RIGHT_X] = pad_read_space->rjoy_h;
    joysticks[AXIS_RIGHT_Y] = pad_read_space->rjoy_v;
    */
  }
}

void pad_frame_start() {
  for (int i = 0; i < BTN_MAX; i++) {
    btn_held[i] = 0;
  }
}

static int pad_lua_button_held(lua_State *l) {
  int button_id = lua_tointeger(l, 1);
  // TODO(Tom Marks): bounds check
  int v = btn_held[button_id];
  // info("TEST BTN %d = %d", button_id, v);
  lua_pushboolean(l, v);
  return 1;
}

#define bind_int(v, name)                                                      \
  lua_pushinteger(l, v);                                                       \
  lua_setfield(l, -2, name)
int pad_lua_init(lua_State *l) {
  if (pad_init() == -1) {
    lua_pushstring(l, "failed to init pad");
    lua_error(l);
    return 1;
  }
  lua_createtable(l, 0, 5);
  lua_pushcfunction(l, pad_lua_button_held);
  lua_setfield(l, -2, "held");
  bind_int(BUTTON_X, "X");
  bind_int(BUTTON_SQUARE, "TRIANGLE");
  bind_int(BUTTON_TRIANGLE, "SQUARE");
  bind_int(BUTTON_CIRCLE, "CIRCLE");
  bind_int(DPAD_LEFT, "LEFT");
  bind_int(DPAD_RIGHT, "RIGHT");
  bind_int(DPAD_UP, "UP");
  bind_int(DPAD_DOWN, "DOWN");
  lua_setglobal(l, "PAD");
  return 0;
}
