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

// easy :D
int draw_clear_buffer() {
  trace("draw clear buffer");
  state.cmdbuffer_head = state.cmdbuffer;
  state.cmdbuffer_head_offset = 0;
  state.gif.head = 0;
  state.dma.head = 0;
  state.dma.in_cnt = 0;
  state.d2d.draw_type = DRAW_FMT_NONE;
  return 1;
}

int draw_update_last_tag_loops() {
  trace("draw update last tag @ %p", state.gif.head);
  if (state.gif.head) {
    trace("draw update last tag loop: %d", state.gif.loop_count);
    if (state.gif.loop_count <= GIF_MAX_LOOPS) {
      uint32_t eop = *state.gif.head & 0x8000;
      *state.gif.head = state.gif.loop_count | eop;
    } else {
      error("too many loops in GIFTag");
    }
  }
  return 1;
}

int draw_start_cnt() {
  trace("start cnt @ %d", state.cmdbuffer_head_offset);
  state.dma.in_cnt = 1;
  state.dma.head = state.cmdbuffer_head;
  dma_tag((uint32_t *)state.cmdbuffer_head, 0, 0x1 << 28, 0);
  state.cmdbuffer_head += QW_SIZE;
  state.cmdbuffer_head_offset += QW_SIZE;
  return 1;
}

int dmatag_raw(int qwc, int type, int addr) {
  dma_tag((uint32_t *)state.cmdbuffer_head, qwc, type, addr);
  state.cmdbuffer_head += QW_SIZE;
  state.cmdbuffer_head_offset += QW_SIZE;
  return 1;
}

int draw_end_cnt() {
  trace("end cnt, state=%d, head=%d", state.dma.in_cnt,
        state.cmdbuffer_head_offset);
  if (state.dma.in_cnt) {
    state.dma.in_cnt = 0;
    size_t dma_len = state.cmdbuffer_head - state.dma.head;
    // if there is nothing in this DMA packet, then rewind one qword..
    if (dma_len == 16) {
      trace("rewinding cnt header");
      state.cmdbuffer_head -= QW_SIZE;
      state.cmdbuffer_head_offset -= QW_SIZE;
      return 1;
    }
    trace("set dma cnt qwc=%d", dma_len / 16);
    uint16_t *lh = (uint16_t *)state.dma.head;
    if (dma_len % 16 == 0) {
      *lh = (dma_len / 16) - 1;
    } else {
      *lh = dma_len / 16;
    }
  }
  return 1;
}

int draw_dma_end() {
  draw_end_cnt();
  dma_tag((uint32_t *)state.cmdbuffer_head, 0, 0x7 << 28, 0);
  state.cmdbuffer_head += QW_SIZE;
  state.cmdbuffer_head_offset += QW_SIZE;
  return 1;
}

int draw_dma_ref(uint32_t addr, int qwc) {
  draw_end_cnt();
  dma_tag((uint32_t *)state.cmdbuffer_head, qwc, 0x3 << 28, addr);
  state.cmdbuffer_head += QW_SIZE;
  state.cmdbuffer_head_offset += QW_SIZE;
  return 1;
}

int draw_kick() {
  trace("kick buffer of size=%d", state.cmdbuffer_head_offset);
  draw_update_last_tag_loops();
  draw_end_cnt();
  draw_dma_end();
  size_t buffer_size = state.cmdbuffer_head - state.cmdbuffer;
  trace("dma send");
  print_buffer((qword_t *)state.cmdbuffer, buffer_size / 16);
  dma_channel_send_chain(DMA_CHANNEL_GIF, state.cmdbuffer, buffer_size / 16, 0,
                         0);
  dma_wait_fast();
  // TODO(phy1um): get new memory?
  draw_clear_buffer();
  draw_start_cnt();
  state.this_frame.kick_count += 1;
  return 1;
}

int draw_frame_start() {
  trace("frame start");
  memset(&state.this_frame, 0, sizeof(struct draw_stats));
  draw_clear_buffer();
  draw_start_cnt();

  // Clear the screen using PS2SDK functions
  float halfw = (state.screen_w * 1.0f) / 2.0f;
  float halfh = (state.screen_h * 1.0f) / 2.0f;
  qword_t *q = (qword_t *)state.cmdbuffer_head;
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
        state.screen_h, 2048.0f - halfw, 2048.0f - halfh, state.cmdbuffer_head,
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
  state.cmdbuffer_head = (char *)q;
  state.cmdbuffer_head_offset = ((char *)q - state.cmdbuffer);
  return 1;
}

int draw_frame_end() {
  trace("frame end");
  qword_t *q = draw_finish((qword_t *)state.cmdbuffer_head);
  state.cmdbuffer_head = (char *)q;
  draw_kick();
  memcpy(&state.last_frame, &state.this_frame, sizeof(struct draw_stats));
  return 1;
}
