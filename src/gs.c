#include <lua.h>

#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <inttypes.h>

#include <p2g/log.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "gs_state.h"

struct gs_state *GS_STATE;
static qword_t *flip_buffer;

int gs_init() {
  info("init GS -- no framebuffer");
  GS_STATE = calloc(1, sizeof(struct gs_state));
  flip_buffer = memalign(64, 10 * 16);
  return 0;
}

int gs_flip() {
  trace("GS FLIP START");
  memset(flip_buffer, 0, 10 * 16);
  framebuffer_t *fb = &GS_STATE->fb[GS_STATE->ctx];
  graph_set_framebuffer_filtered(fb->address, fb->width, fb->psm, 0, 0);
  GS_STATE->ctx ^= 1;
  trace("GS BUILD FLIP BUFFER");
  qword_t *q = flip_buffer;
  q = draw_framebuffer(q, 0, &GS_STATE->fb[GS_STATE->ctx]);
  q = draw_finish(q);
  dma_wait_fast();
  trace("GS SEND FLIP BUFFER");
  dma_channel_send_normal(2, flip_buffer, q - flip_buffer, 0, 0);
  trace("GS WAIT FLIP BUFFER DRAW FINISH");
  draw_wait_finish();
  trace("GS FLIP DONE");
  return 0;
}

int gs_set_fields(int width, int height, int fmt, int zfmt, int fb1_addr,
    int fb2_addr, int zbuf_addr) {
  trace("setup GS fields: fb1 @ %d, fb2 @ %d, zbuffer @ %d", fb1_addr,
      fb2_addr, zbuf_addr);
  struct gs_state *st = GS_STATE;
  st->fb[0].address = fb1_addr;
  st->fb[0].width = width;
  st->fb[0].height = height;
  st->fb[0].psm = fmt;
  st->fb[0].mask = 0;
  st->fb[1].address = fb2_addr;
  st->fb[1].width = width;
  st->fb[1].height = height;
  st->fb[1].psm = fmt;
  st->fb[1].mask = 0;

  st->zb.enable = 1;
  st->zb.address = zbuf_addr;
  st->zb.zsm = zfmt;
  st->zb.method = ZTEST_METHOD_GREATER_EQUAL;
  st->zb.mask = 0;
  graph_set_framebuffer_filtered(fb2_addr, width, fmt, 0, 0);
  graph_enable_output();

  // init draw state
  qword_t *head = malloc(20 * 16);
  memset(head, 0, 20 * 16);
  qword_t *q = head;
  q = draw_setup_environment(q, 0, st->fb, &st->zb);
  q = draw_primitive_xyoffset(q, 0, 2048 - (width / 2),
                              2048 - (height / 2));
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, head, q - head, 0, 0);
  draw_wait_finish();
  free(head);

  return 0;
}

int gs_set_output(int width, int height, int interlace, int mode, int ffmd,
    int filter_flicker) {
  graph_disable_output();
  graph_set_mode(interlace, mode, ffmd, filter_flicker);
  graph_set_screen(0, 0, width, height);
  graph_set_bgcolor(0, 0, 0);
  return 1;
}

int gs_framebuffer_size(int width, int height, int psm) {
  int size = width*height;
  switch (psm) {
    case GS_PSM_32:
    case GS_PSMZ_32:
      size *= 4;
      break;
    case GS_PSM_24:
      size *= 3;
      break;
    case GS_PSM_16:
    case GS_PSMZ_16:
    case GS_PSM_16S:
      size *= 2;
      break;
    case GS_PSM_4:
      size = (int)((float)size*0.5);
      break;
  }
  return size;
}
