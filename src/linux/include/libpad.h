
#ifndef P2SIM_LIBPAD_H
#define P2SIM_LIBPAD_H

enum pad {
  PAD_STATE_STABLE,
  PAD_STATE_FINDCTP1,
  PAD_STATE_DISCONN,
  PAD_MODETABLE,
  PAD_TYPE_DUALSHOCK,
  PAD_RSTAT_COMPLETE,
};

enum pad_buttons {
  PAD_LEFT,
  PAD_RIGHT,
  PAD_UP,
  PAD_DOWN,
  PAD_SQUARE,
  PAD_TRIANGLE,
  PAD_CIRCLE,
  PAD_CROSS,
  PAD_L1,
  PAD_L2,
  PAD_R1,
  PAD_R2,
  PAD_START,
  PAD_SELECT,
};

enum pad_axis {
  AXIS_LEFT_X,
  AXIS_LEFT_Y,
  AXIS_RIGHT_X,
  AXIS_RIGHT_Y,
};

struct padButtonStatus {
  int btns;
  int ljoy_h;
  int ljoy_v;
  int rjoy_h;
  int rjoy_v;
};


void padInit(int i);
void padPortOpen(int port, int slot, void *p);
int32_t padGetState(int port, int slot);
int padInfoMode(int port, int slot, int mode, int i);
void padSetMainMode(int port, int slot, int a, int b);
int padGetReqState(int port, int slot);

int padRead(int port, int slot, void *p);




#endif
