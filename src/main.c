#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dma.h>
#include <dma_tags.h>
#include <draw.h>
#include <graph.h>

#include <inttypes.h>

#include "drawstate.h"
#include "gs.h"
#include "log.h"
#include "mesh.h"
#include "pad.h"
#include "ps2draw.h"
#include "script.h"

#define OFFSET_X 2048
#define OFFSET_Y 2048

#define VID_W 640
#define VID_H 448

#define TGT_FILE "host:MIT_teapot.bin"

#define fatalerror(st, msg, ...)                                               \
  printf("FATAL: " msg "\n", ##__VA_ARGS__);                                   \
  error_forever(st);                                                           \
  ((void)0)
void error_forever(struct draw_state *st);

int print_buffer(qword_t *b, int len) {
  printf("-- buffer\n");
  for (int i = 0; i < len; i++) {
    printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
    b++;
  }
  printf("-- /buffer\n");
  return 0;
}

int main() {

  printf("Hello\n");
  qword_t *buf = malloc(20000 * 16);
  char *file_load_buffer = malloc(310 * 1024);
  int file_load_buffer_len = 310 * 1024;

  // init DMAC
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  void *lua = script_load("host:entry.lua");
  script_simple_call(lua, "ps2_init");

  struct model m = {0};
  m.r = 0xff;
  int bytes_read = load_file(TGT_FILE, file_load_buffer, file_load_buffer_len);
  if (bytes_read <= 0) {
    fatalerror(drawstate_gs_state(), "failed to load file %s", TGT_FILE);
  }
  if (bytes_read % 16 != 0) {
    fatalerror(drawstate_gs_state(), "lengt of model file %s was not 0 %% 16",
               TGT_FILE);
  }

  if (!model_load(&m, file_load_buffer, bytes_read)) {
    fatalerror(drawstate_gs_state(), "failed to process model");
  }

  struct draw_state *draw_state = drawstate_gs_state();
  struct render_state *r = drawstate_get();

  draw_state->clear_col[0] = 0xb1;
  draw_state->clear_col[1] = 0xce;
  draw_state->clear_col[2] = 0xcb;

  r->offset_x = OFFSET_X;
  r->offset_y = OFFSET_Y;

  struct model_instance inst = {0};
  inst.m = &m;
  inst.scale[0] = 1.f;
  inst.scale[1] = 1.f;
  inst.scale[2] = 1.f;
  inst.scale[3] = 1.0f;
  inst.translate[2] = 10.f;

  pad_init();

  graph_wait_vsync();

  while (1) {

    pad_frame_start();
    pad_poll();
    update_draw_matrix(&r);
    dma_wait_fast();

    qword_t *q = buf;
    memset(buf, 0, 20000 * 16);

    q = gs_frame_start(draw_state, q);

    if (mesh_is_visible(&inst, r)) {
      qword_t *model_verts_start = q;
      memcpy(q, m.buffer, m.buffer_len);
      mesh_transform((char *)(model_verts_start + MESH_HEADER_SIZE), &inst, r);
      q += (m.buffer_len / 16);
    }

    q = gs_frame_end(draw_state, q);

    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q - buf, 0, 0);

    draw_wait_finish();
    graph_wait_vsync();

#if 0
    unsigned char joyx = joy_axis_value(AXIS_LEFT_X);
    float dx = (joy_axis_value(AXIS_LEFT_X) - 128) / 128.0f;
    if ( fabs(dx) < 0.2f ) { dx = 0; }
    float dz = (joy_axis_value(AXIS_LEFT_Y) - 128) / 128.0f;
    if ( fabs(dz) < 0.2f ) { dz = 0; }
    int dy = button_held(DPAD_DOWN) - button_held(DPAD_UP);

    info("joy %f,%f", dx, dz);
#else
    int dx = button_held(DPAD_RIGHT) - button_held(DPAD_LEFT);
    int dz = button_held(DPAD_DOWN) - button_held(DPAD_UP);
    int dy = button_held(BUTTON_L1) - button_held(BUTTON_L2);
#endif

    r->camera_pos[0] += 0.2f * dx;
    r->camera_pos[2] += 0.2f * dz;
    // r->camera_pos[1] += 0.1f * dy;
    r->camera_rotate_y += 0.01f * dy;
  }
}

void error_forever(struct draw_state *st) {
  qword_t *buf = malloc(1200);
  while (1) {
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, 1200);
    q = draw_disable_tests(q, 0, &st->zb);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, VID_W, VID_H, 0xff, 0,
                   0);
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q - buf, 0, 0);
    draw_wait_finish();
    graph_wait_vsync();
    sleep(2);
  }
}

/*
 *
 * xxxx xxxx xxxx . yyyy
 * 0100 0000 0000 . 0000
 *
2 */
