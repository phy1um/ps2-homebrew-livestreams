
#ifndef PAD_H
#define PAD_H

#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_L1 2
#define BUTTON_R1 3
#define BUTTON_L2 4
#define BUTTON_R2 5
#define DPAD_DOWN 6
#define DPAD_LEFT 7
#define DPAD_RIGHT 8
#define DPAD_UP 9
#define BTN_MAX 10

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
unsigned char joy_axis_value(int a);

int pad_init();
void pad_poll();
void pad_frame_start();

#endif
