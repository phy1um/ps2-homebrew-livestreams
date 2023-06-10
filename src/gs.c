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
  dma_channel_send_normal(DMA_CHANNEL_GIF, flip_buffer, q - flip_buffer, 0, 0);
  trace("GS WAIT FLIP BUFFER DRAW FINISH");
  draw_wait_finish();
  trace("GS FLIP DONE");
  return 0;
}

