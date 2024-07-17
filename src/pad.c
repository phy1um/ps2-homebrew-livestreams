#include <gs_privileged.h>
#include <kernel.h>
#include <libpad.h>
#include <loadfile.h>

#include <malloc.h>

#include <p2g/log.h>
#include <p2g/pad.h>

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


