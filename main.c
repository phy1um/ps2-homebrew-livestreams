#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <dma.h>
#include <dma_tags.h>

#include <inttypes.h>

#include "gs.h"
#include "mesh.h"
#include "draw.h"
#include "log.h"
#include "pad.h"

#define OFFSET_X 2048
#define OFFSET_Y 2048


#define VID_W 640
#define VID_H 448

#define TGT_FILE "host:cube.bin"

#define fatalerror(st, msg, ...) printf("FATAL: " msg "\n", ##__VA_ARGS__); error_forever(st); ((void)0)
void error_forever(struct draw_state *st);

int print_buffer(qword_t *b, int len)
{
  printf("-- buffer\n");
  for(int i = 0; i < len; i++) {
    printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
    b++;
  }
  printf("-- /buffer\n");
  return 0;
}

int main()
{
  printf("Hello\n");
  qword_t *buf = malloc(20000*16);
  char *file_load_buffer = malloc(310 * 1024);
  int file_load_buffer_len = 310*1024;

  struct draw_state st = {0};
  st.width = VID_W,
  st.height = VID_H,
  st.vmode = graph_get_region(),
  st.gmode = GRAPH_MODE_INTERLACED,

  // init DMAC
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  // initialize graphics mode 
  gs_init(&st, GS_PSM_32, GS_PSMZ_24);

  struct model m = {0};
  m.r = 0xff;
  int bytes_read = load_file(TGT_FILE, file_load_buffer, file_load_buffer_len);
  if (bytes_read <= 0) {
    fatalerror(&st, "failed to load file %s", TGT_FILE);
  }
  if (bytes_read % 16 != 0) {
    fatalerror(&st, "lengt of model file %s was not 0 %% 16", TGT_FILE);
  }

  if (!model_load(&m, file_load_buffer, bytes_read)) {
    fatalerror(&st, "failed to process model");
  }

  struct render_state r = {0};

  r.camera_pos[0] = 0.0f;
  r.camera_pos[2] = 10.0f;
  r.camera_pos[3] = 1.0f;

  r.clear_col[0] = 0xb1;
  r.clear_col[1] = 0xce;
  r.clear_col[2] = 0xcb;

  r.offset_x = OFFSET_X;
  r.offset_y = OFFSET_Y;

  struct model_instance inst = {0};
  inst.m = &m;
  inst.scale[0] = .8f;
  inst.scale[1] = .8f;
  inst.scale[2] = .8f;
  inst.scale[3] = 1.0f;

  pad_init();

  graph_wait_vsync();
  while(1) {
    pad_frame_start();
    pad_poll();
    update_draw_matrix(&r);
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, 20000*16);
    q = draw_disable_tests(q, 0, &st.zb);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, 
        VID_W, VID_H, 
        r.clear_col[0], r.clear_col[1], r.clear_col[2]);
    q = draw_enable_tests(q, 0, &st.zb);
    qword_t *model_verts_start = q;
    memcpy(q, m.buffer, m.buffer_len);
    // info("copied mesh buffer with len=%d", m.buffer_len);
    q += (m.buffer_len/16);
    q = draw_finish(q);
    mesh_transform((char*) (model_verts_start+MESH_HEADER_SIZE), &inst, &r);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
    // print_buffer(buf, q-buf); 
    // info("draw from buffer with length %d", q-buf);

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
#endif

    r.camera_pos[0] += 0.02f * dx;
    r.camera_pos[2] += 0.02f * dz;

  }
}


void error_forever(struct draw_state *st)
{
  qword_t *buf = malloc(1200);
  while(1) {
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, 1200);
    q = draw_disable_tests(q, 0, &st->zb);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, VID_W, VID_H, 0xff, 0, 0);
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
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
