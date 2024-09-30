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
#include "internal.h"

struct render_state state = {0};

static int command_buffer_align_head(struct commandbuffer *c, size_t b) {
  int old_offset = c->offset;
  while (c->offset % b != 0) {
    c->head += 1;
    c->offset += 1;
  }
  trace("align buffer head to %d bytes, %d -> %d", b, old_offset, c->offset);
  return 0;
}

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
  trace("draw clear buffer (%p %d)", state.buffer.ptr, state.buffer.offset);
  clear_command_buffer(&state.buffer);
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

int draw_vifcode_direct_start(struct commandbuffer *c) {
  trace("start direct vifcode @ %d", c->offset);
  command_buffer_align_head(c, 16);
  vifcode((uint32_t *)c->head, VIF_CODE_NOP, VIF_CODE_NO_STALL, 0, 0);
  vifcode((uint32_t *)c->head + 4, VIF_CODE_NOP, VIF_CODE_NO_STALL, 0, 0);
  vifcode((uint32_t *)c->head + 8, VIF_CODE_NOP, VIF_CODE_NO_STALL, 0, 0);
  // TODO(phy1um): check if we are the first vifcode in a dma transfer first?
  c->head += 12;
  c->offset += 12;
  c->vif.head = c->head;
  c->vif.is_direct_gif = 1;
  c->vif.is_active = 1;
  vifcode((uint32_t *)c->vif.head, VIF_CODE_DIRECT, VIF_CODE_NO_STALL, 0, 0);
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  return 1;
}

int draw_start_cnt(struct commandbuffer *c) {
  command_buffer_align_head(c, 16);
  trace("start cnt @ %d", c->offset);
  c->dma.in_cnt = 1;
  c->dma.head = c->head;
  dma_tag((uint32_t *)c->head, 0, 0x1 << 28, 0);
  c->head += sizeof(uint64_t);
  c->offset += sizeof(uint64_t);
  return 1;
}

int dmatag_raw(struct commandbuffer *c, int qwc, int type, int addr) {
  command_buffer_align_head(c, 16);
  dma_tag((uint32_t *)c->head, qwc, type, addr);
  c->head += sizeof(uint64_t);
  c->offset += sizeof(uint64_t);
  return 1;
}

int draw_giftags_begin(struct commandbuffer *c) {
  trace("begin giftags buffer@=%d", c->offset);
  if (c->target_vif) {
    if (c->vif.is_active && c->vif.is_inline_unpack) {
      trace("giftags in an inline unpack..");
      command_buffer_align_head(c, 16);
      return 1;
    }
    if (c->vif.is_active && !c->vif.is_direct_gif) {
      trace("end previous vifcode before starting giftags buffer@=%d",
            c->offset);
      // end previous vifcode
      draw_vifcode_end(c);
    }
    // start next direct tag
    draw_vifcode_direct_start(&state.buffer);
  }
  return 1;
}

int draw_vifcode_end(struct commandbuffer *c) {
  trace("end vifcode, direct=%d, unpack_inline=%d, buffer@=%d",
        c->vif.is_direct_gif, c->vif.is_inline_unpack, c->offset);
  if (!c->target_vif || !c->vif.is_active) {
    return 1;
  }
  size_t packet_len = c->head - c->vif.head;
  if (packet_len == sizeof(uint32_t)) {
    trace("rewinding vifcode of empty packet");
    c->head -= sizeof(uint32_t);
    c->offset -= sizeof(uint32_t);
    return 1;
  }
  if (c->vif.is_direct_gif) {
    commandbuffer_update_last_tag_loop(c);
    int qwc = (packet_len) / (QW_SIZE);
    if (packet_len % QW_SIZE != 0) {
      qwc += 1;
    }
    if (qwc >= 65536) {
      // TODO(phy1um): decide how to handle large vifcodes, probably before
      // this point
      logerr("vifcode is too big!!!");
      return 1;
    }
    int tag_offset = (int)(c->vif.head - c->ptr);
    trace("vifcode set imm = %d, buffer@=%d (size in bytes = %d)", qwc - 2,
          tag_offset, packet_len);
    vifcode_update_imm((uint16_t *)c->vif.head, qwc);
    c->vif.is_direct_gif = 0;
  } else if (c->vif.is_inline_unpack) {
    draw_vu_end_unpack_inline(c, packet_len);
  } else {
    logerr("unsupported VIF transfer");
  }
  c->vif.is_active = 0;
  return 1;
}

int draw_end_cnt(struct commandbuffer *c) {
  trace("end cnt, state=%d, head=%d", c->dma.in_cnt, c->offset);
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
  trace("dma end tag buffer@=%d", c->offset);
  command_buffer_align_head(&state.buffer, 16);
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

int draw_kick() {
  struct commandbuffer *c = &state.buffer;
  if (c->target_vif) {
    return draw_kick_vif(c);
  } else {
    return draw_kick_gif(c);
  }
}

int draw_kick_gif(struct commandbuffer *c) {
  trace("kick gif: buffer of size=%d", c->offset);
  commandbuffer_update_last_tag_loop(c);
  draw_dma_end(c);
  size_t buffer_size = c->head - c->ptr;
  trace("kick gif: send");
  print_buffer((qword_t *)c->ptr, buffer_size / 16);
  dma_channel_send_chain(DMA_CHANNEL_GIF, c->ptr, buffer_size / 16, 0, 0);
  trace("kick gif: dma wait fast");
  // TODO(phy1um): fix this wait...
  dma_wait_fast();
  // TODO(phy1um): get new memory?
  clear_command_buffer(c);
  draw_start_cnt(c);
  // TODO(phy1um): kick count vif vs gif
  state.this_frame.kick_count += 1;
  return 1;
}

int draw_kick_vif(struct commandbuffer *c) {
  trace("kick vif: buffer of size=%d", c->offset);
  commandbuffer_update_last_tag_loop(c);
  draw_vifcode_end(c);
  draw_dma_end(c);
  size_t buffer_size = c->head - c->ptr;
  trace("kick vif: dma send TO VIF AND NOT GIF!!!!");
  print_buffer((qword_t *)c->ptr, buffer_size / 16);
  dma_channel_send_chain(0x1, c->ptr, buffer_size / 16, 0, 0);
  trace("kick vif: dma wait fast");
  dma_wait_fast();
  // TODO(phy1um): get new memory?
  clear_command_buffer(c);
  draw_start_cnt(c);
  // TODO(phy1um): kick count vif vs gif
  state.this_frame.kick_count += 1;
  return 1;
}

int draw_frame_start() {
  trace("frame start: %p (%d)", state.buffer.ptr, state.buffer.offset);
  state.buffer.in_frame = 1;
  memset(&state.this_frame, 0, sizeof(struct draw_stats));
  draw_clear_buffer();
  draw_start_cnt(&state.buffer);
  if (state.buffer.target_vif) {
    draw_vifcode_direct_start(&state.buffer);
  }

  // Clear the screen using PS2SDK functions
  float halfw = (state.screen_w * 1.0f) / 2.0f;
  float halfh = (state.screen_h * 1.0f) / 2.0f;
  command_buffer_align_head(&state.buffer, 16);
  qword_t *q = (qword_t *)state.buffer.head;
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
        state.screen_h, 2048.0f - halfw, 2048.0f - halfh, state.buffer.head, q);
  // enable depth tests
  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, GS_STATE->zb.method),
              GS_REG_TEST);
  q++;
  state.buffer.head = (char *)q;
  state.buffer.offset = ((char *)q - state.buffer.ptr);
  return 1;
}

int draw_frame_end() {
  trace("frame end: in");
  commandbuffer_update_last_tag_loop(&state.buffer);
  draw_giftags_begin(&state.buffer);
  trace("frame end: AD draw finish buffer@=%d", state.buffer.offset);
  giftag_new(&state.buffer, 0, 1, 1, 0x1, 0xe);
  gif_ad(&state.buffer, 0x61, 1);
  draw_kick();
  trace("frame end: /kick");
  memcpy(&state.last_frame, &state.this_frame, sizeof(struct draw_stats));
  state.buffer.in_frame = 0;
  return 1;
}

int draw_vu_end_unpack_inline(struct commandbuffer *c, size_t packet_size) {
  commandbuffer_update_last_tag_loop(c);
  size_t qword_size = packet_size / 16;
  int buffer_position = (int)(c->vif.head - c->ptr);
  trace("vu inline unpack end: update num = %d @buffer pos=%d", qword_size,
        buffer_position);
  vifcode_update_num((uint8_t *)c->vif.head, qword_size);
  c->vif.is_inline_unpack = 0;
  return 1;
}

int draw_set_target(int target_vif) {
  if (state.buffer.in_frame) {
    logerr("cannot set VIF target = %d during a frame", target_vif);
    return 0;
  }
  state.buffer.target_vif = target_vif;
  return 1;
}
