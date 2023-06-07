#include <lua.h>

#include <gs_privileged.h>
#include <kernel.h>
#include <libpad.h>
#include <loadfile.h>

#include <malloc.h>

#include <p2g/log.h>
#include <p2g/pad.h>

#define DEADZONE 0.2

#define pad_test(b, v)                                                         \
  do {                                                                         \
    if ((c & b)) {                                                             \
      btn_held[v] = 1;                                                         \
    }                                                                          \
  } while (0)

char *pad_buffer;
struct padButtonStatus *pad_read_space;

int *btn_held;

int joysticks[JOY_AXIS_COUNT];

int prev_inputs;

int button_held(int b) { return btn_held[b]; }

int joy_axis_value(int a) { return joysticks[a]; }

static void wait_vblank() {
  // Enable the vsync interrupt.
  *GS_REG_CSR |= GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);

  // Wait for the vsync interrupt.
  while (!(*GS_REG_CSR & (GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)))) {
  }

  // Disable the vsync interrupt.
  *GS_REG_CSR &= ~GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
}

int pad_init() {
  btn_held = memalign(BTN_MAX * sizeof(int), 256);
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
  int busy_loops = 100;
  while (busy_loops > 0) {
    int32_t state = padGetState(0, 0);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
      int modes = padInfoMode(0, 0, PAD_MODETABLE, -1);
      for (int i = 0; i < modes; i++) {
        if (padInfoMode(0, 0, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) {
          info("found dualshock controller in 0:0");
        }
      }
      padSetMainMode(0, 0, 1, 3);
      while (padGetReqState(0, 0) != PAD_RSTAT_COMPLETE) {
      }
      while (padGetState(0, 0) != PAD_STATE_STABLE) {
      }
      return 1;
    }
    busy_loops -= 1;
    wait_vblank();
  }

  logerr("failed to set pad mode, state never stable");
  return 0;
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
    pad_test(PAD_SELECT, BUTTON_SELECT);
    pad_test(PAD_START, BUTTON_START);
    joysticks[AXIS_LEFT_X] = (int)pad_read_space->ljoy_h;
    joysticks[AXIS_LEFT_Y] = (int)pad_read_space->ljoy_v;
    joysticks[AXIS_RIGHT_X] = (int)pad_read_space->rjoy_h;
    joysticks[AXIS_RIGHT_Y] = (int)pad_read_space->rjoy_v;
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

static int pad_lua_joy_value(lua_State *l) {
  int axis_id = lua_tointeger(l, 1);
  // logdbg("axis %d = %d", axis_id, joysticks[axis_id]);
  float joy_value = ((joysticks[axis_id] - 127) * 1.0) / 127.0;
  if ((joy_value < 0 && joy_value > -DEADZONE) ||
      (joy_value > 0 && joy_value < DEADZONE)) {
    lua_pushinteger(l, 0);
  } else {
    lua_pushnumber(l, joy_value);
  }
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
  lua_pushcfunction(l, pad_lua_joy_value);
  lua_setfield(l, -2, "axis");
  bind_int(BUTTON_X, "X");
  bind_int(BUTTON_SQUARE, "SQUARE");
  bind_int(BUTTON_TRIANGLE, "TRIANGLE");
  bind_int(BUTTON_CIRCLE, "CIRCLE");
  bind_int(DPAD_LEFT, "LEFT");
  bind_int(DPAD_RIGHT, "RIGHT");
  bind_int(DPAD_UP, "UP");
  bind_int(DPAD_DOWN, "DOWN");
  bind_int(BUTTON_SELECT, "SELECT");
  bind_int(BUTTON_START, "START");
  bind_int(BUTTON_L1, "L1");
  bind_int(BUTTON_L2, "L2");
  bind_int(BUTTON_R1, "R1");
  bind_int(BUTTON_R2, "R2");
  bind_int(AXIS_LEFT_X, "axisLeftX");
  bind_int(AXIS_LEFT_Y, "axisLeftY");
  bind_int(AXIS_RIGHT_X, "axisRightX");
  bind_int(AXIS_RIGHT_Y, "axisRightY");
  return 1;
}
