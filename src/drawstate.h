
#ifndef SRC_DRAWSTATE_H
#define SRC_DRAWSTATE_H

#include "ps2draw.h"
#include "gs.h"

#include <tamtypes.h>

int drawstate_init(int w, int h, int i);
int drawstate_camera_step(float x, float y, float z);
int drawstate_camera_jump(float x, float y, float z);
int drawstate_camera_rotate(float x, float y, float z);
qword_t * drawstate_ztest(qword_t *q, int enable);
struct render_state *drawstate_get();
struct draw_state *drawstate_gs_state();

#endif
