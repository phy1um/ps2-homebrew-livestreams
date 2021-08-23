
#include <libpad.h>
#include <loadfile.h>
#include <kernel.h>

#include "pad.h"
#include "log.h"

#define pad_test(b, v) \
  do { if ( (c&b) ) { \
    btn_held[v] = 1; \
  } } while(0)

static char pad_buffer[256];
static struct padButtonStatus pad_read_space = {0};

static int btn_held[BTN_MAX];

static unsigned char joysticks[JOY_AXIS_COUNT];

static int prev_inputs;

int button_held(int b)
{
  return btn_held[b];
}

unsigned char joy_axis_value(int a)
{
  return joysticks[a];
}

int pad_init()
{
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
  padPortOpen(0, 0, &pad_buffer);
  return 1;
}

static int pad_wait(int port, int slot, int tries)
{
  int now;
  int prev = -1;
  now = padGetState(port, slot);
  if ( now == PAD_STATE_DISCONN ) {
    return -1;
  }
  while ( (now != PAD_STATE_STABLE) && (now != PAD_STATE_FINDCTP1) ) {
    prev = now;
    now = padGetState(port, slot);
    tries--;
    if ( tries == 0 ) {
      break;
    }
  }
  return 0;
}

void pad_poll()
{
  if ( pad_wait(0,0,10) < 0 ) {
    return;
  }

  if ( padRead(0, 0, &pad_read_space) != 0 ) {
    int pad = 0xffff ^ pad_read_space.btns;
    int c = pad ^ prev_inputs;
    pad_test(PAD_LEFT, DPAD_LEFT);
    pad_test(PAD_RIGHT, DPAD_RIGHT);
    pad_test(PAD_UP, DPAD_UP);
    pad_test(PAD_DOWN, DPAD_DOWN);
    pad_test(PAD_CROSS, BUTTON_1);
    pad_test(PAD_SQUARE, BUTTON_2);
    pad_test(PAD_L1, BUTTON_L1);
    pad_test(PAD_L2, BUTTON_L2);
    pad_test(PAD_R1, BUTTON_R1);
    pad_test(PAD_R2, BUTTON_R2);
    joysticks[AXIS_LEFT_X] = pad_read_space.ljoy_h;
    joysticks[AXIS_LEFT_Y] = pad_read_space.ljoy_v;
    joysticks[AXIS_RIGHT_X] = pad_read_space.rjoy_h;
    joysticks[AXIS_RIGHT_Y] = pad_read_space.rjoy_v;
  }
}

void pad_frame_start()
{
  for(int i = 0; i < BTN_MAX; i++) {
    btn_held[i] = 0;
  }
}


