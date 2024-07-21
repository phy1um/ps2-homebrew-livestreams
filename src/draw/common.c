#include <dma.h>
#include <dma_tags.h>
#include <draw_tests.h>
#include <gif_tags.h>
#include <gs_gp.h>
#include <gs_psm.h>
#include <tamtypes.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <p2g/log.h>

#include "../gs_state.h"
#include "buffer.h"
#include "draw.h"

struct render_state state = {0};

static int clear_command_buffer(struct commandbuffer *c) {
  c->head = c->ptr;
  c->offset = 0;
  c->gif.head = 0;
  c->dma.head = 0;
  c->dma.in_cnt = 0;
  return 0;
}

int commandbuffer_update_last_tag_loop(struct commandbuffer *c) {
  trace("draw update last tag @ %p", c->gif.head);
  if (c->gif.head) {
    trace("draw update last tag loop: %d", c->gif.loop_count);
    if (c->gif.loop_count <= GIF_MAX_LOOPS) {
      uint32_t eop = *c->gif.head & 0x8000;
      *c->gif.head = c->gif.loop_count | eop;
    } else {
      error("too many loops in GIFTag");
    }
  }
  return 1;
}

// easy :D
int draw_clear_buffer() {
  trace("draw clear buffer");
  clear_command_buffer(&state.vif_buffer);
  clear_command_buffer(&state.gif_buffer);
  state.d2d.draw_type = DRAW_FMT_NONE;
  return 1;
}

/*
int draw_update_last_tag_loops() {
  int rv = commandbuffer_update_last_tag_loop(&state.vif_buffer);
  rv |= commandbuffer_update_last_tag_loop(&state.gif_buffer);
  return rv;
}
*/

int draw_start_cnt(struct commandbuffer *c) {
  trace("start cnt @ %d", c->offset);
  c->dma.in_cnt = 1;
  c->dma.head = c->head;
  dma_tag((uint32_t *)c->head, 0, 0x1 << 28, 0);
  c->head += QW_SIZE;
  c->offset += QW_SIZE;
  return 1;
}

int dmatag_raw(struct commandbuffer *c, int qwc, int type, int addr) {
  dma_tag((uint32_t *)c->head, qwc, type, addr);
  c->head += QW_SIZE;
  c->offset += QW_SIZE;
  return 1;
}

int draw_end_cnt(struct commandbuffer *c) {
  trace("end cnt, state=%d, head=%d", c->dma.in_cnt,
        c->offset);
  if (c->dma.in_cnt) {
    c->dma.in_cnt = 0;
    size_t dma_len = c->head - c->dma.head;
    // if there is nothing in this DMA packet, then rewind one qword..
    if (dma_len == 16) {
      trace("rewinding cnt header");
      c->head -= QW_SIZE;
      c->offset -= QW_SIZE;
      return 1;
    }
    trace("set dma cnt qwc=%d", dma_len / 16);
    uint16_t *lh = (uint16_t *)c->dma.head;
    if (dma_len % 16 == 0) {
      *lh = (dma_len / 16) - 1;
    } else {
      *lh = dma_len / 16;
    }
  }
  return 1;
}

int draw_dma_end(struct commandbuffer *c) {
  draw_end_cnt(c);
  dma_tag((uint32_t *)c->head, 0, 0x7 << 28, 0);
  c->head += QW_SIZE;
  c->offset += QW_SIZE;
  return 1;
}

int draw_dma_ref(struct commandbuffer *c, uint32_t addr, int qwc) {
  draw_end_cnt(c);
  dma_tag((uint32_t *)c->head, qwc, 0x3 << 28, addr);
  c->head += QW_SIZE;
  c->offset += QW_SIZE;
  return 1;
}

int draw_kick_gif() {
  trace("kick gif buffer of size=%d", state.gif_buffer.offset);
  commandbuffer_update_last_tag_loop(&state.gif_buffer);
  draw_end_cnt(&state.gif_buffer);
  draw_dma_end(&state.gif_buffer);
  size_t buffer_size = state.gif_buffer.head - state.gif_buffer.ptr;
  trace("dma send");
  print_buffer((qword_t *)state.gif_buffer.ptr, buffer_size / 16);
  dma_channel_send_chain(DMA_CHANNEL_GIF, state.gif_buffer.ptr, buffer_size / 16, 0,
                         0);
  dma_wait_fast();
  // TODO(phy1um): get new memory?
  clear_command_buffer(&state.gif_buffer);
  draw_start_cnt(&state.gif_buffer);
  // TODO: kick count vif vs gif
  state.this_frame.kick_count += 1;
  return 1;
}

int draw_kick_vif() {
  trace("kick vif buffer of size=%d", state.vif_buffer.offset);
  commandbuffer_update_last_tag_loop(&state.vif_buffer);
  draw_end_cnt(&state.vif_buffer);
  draw_dma_end(&state.vif_buffer);
  size_t buffer_size = state.vif_buffer.head - state.vif_buffer.ptr;
  trace("dma send");
  print_buffer((qword_t *)state.vif_buffer.ptr, buffer_size / 16);
  dma_channel_send_chain(DMA_CHANNEL_VIF1, state.vif_buffer.ptr, buffer_size / 16, 0,
                         0);
  dma_wait_fast();
  // TODO(phy1um): get new memory?
  clear_command_buffer(&state.vif_buffer);
  draw_start_cnt(&state.vif_buffer);
  // TODO: kick count vif vs gif
  state.this_frame.kick_count += 1;
  return 1;
}

int draw_frame_start() {
  trace("frame start");
  memset(&state.this_frame, 0, sizeof(struct draw_stats));
  draw_clear_buffer();
  draw_start_cnt(&state.gif_buffer);
  draw_start_cnt(&state.vif_buffer);

  // Clear the screen using PS2SDK functions
  float halfw = (state.screen_w * 1.0f) / 2.0f;
  float halfh = (state.screen_h * 1.0f) / 2.0f;
  qword_t *q = (qword_t *)state.gif_buffer.head;
  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, ZTEST_METHOD_ALLPASS),
              GS_REG_TEST);
  q++;
  q = draw_clear(q, 0, 2048.0f - halfw, 2048.0f - halfh, state.screen_w,
                 state.screen_h, state.clear[0], state.clear[1],
                 state.clear[2]);
  trace("clear screen: %d, %d, %f, %f, %p -> %p", state.screen_w,
        state.screen_h, 2048.0f - halfw, 2048.0f - halfh, state.gif_buffer.head,
        q);
  // enable depth tests
  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, GS_STATE->zb.method),
              GS_REG_TEST);
  q++;
  state.gif_buffer.head = (char *)q;
  state.gif_buffer.offset = ((char *)q - state.gif_buffer.ptr);
  return 1;
}

int draw_frame_end() {
  trace("frame end");
  qword_t *q = draw_finish((qword_t *)state.gif_buffer.head);
  state.gif_buffer.head = (char *)q;
  draw_kick_gif();
  memcpy(&state.last_frame, &state.this_frame, sizeof(struct draw_stats));
  return 1;
}

