
#include <gs_gp.h>
#include <gs_psm.h>

#include "drawstate.h"
#include "gs.h"
#include "ps2draw.h"

static struct draw_state st = {0};
static struct render_state r = {0};

int drawstate_init(int w, int h, int i) {
  st.width = w;
  st.height = h;
  st.vmode = graph_get_region();
  st.gmode = i;
  return gs_init(&st, GS_PSM_32, GS_PSMZ_24);
}

int drawstate_camera_step(float x, float y, float z) {
  r.camera_pos[0] += x;
  r.camera_pos[1] += y;
  r.camera_pos[2] += z;
}

int drawstate_camera_jump(float x, float y, float z) {
  r.camera_pos[0] = x;
  r.camera_pos[1] = y;
  r.camera_pos[2] = z;
}

int drawstate_camera_rotate(float x, float y, float z) {
  r.camera_rotate_y += y;
}

qword_t *drawstate_ztest(qword_t *q, int enable) {
  if (enable) {
    return draw_enable_tests(q, 0, &st.zb);
  } else {
    return draw_disable_tests(q, 0, &st.zb);
  }
}

struct render_state *drawstate_get() {
  return &r;
}

struct draw_state *drawstate_gs_state() {
  return &st;
}
