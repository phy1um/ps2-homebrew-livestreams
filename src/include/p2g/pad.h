
#ifndef PAD_H
#define PAD_H

#include <lua.h>

#define BUTTON_X 0
#define BUTTON_SQUARE 1
#define BUTTON_TRIANGLE 2
#define BUTTON_CIRCLE 3
#define BUTTON_L1 4
#define BUTTON_R1 5
#define BUTTON_L2 6
#define BUTTON_R2 7
#define DPAD_DOWN 8
#define DPAD_LEFT 9
#define DPAD_RIGHT 10
#define DPAD_UP 11
#define BUTTON_SELECT 12
#define BUTTON_START 13
#define BTN_MAX 14

#define AXIS_LEFT_X 0
#define AXIS_LEFT_Y 1
#define AXIS_RIGHT_X 2
#define AXIS_RIGHT_Y 3
#define JOY_AXIS_COUNT 4

#define R_SIO2MAN "rom0:SIO2MAN"
#define R_PADMAN "rom0:PADMAN"

int button_pressed(int b);
int button_held(int b);
int button_released(int b);
int joy_axis_value(int a);

int pad_init();
void pad_poll();
void pad_frame_start();

#endif
