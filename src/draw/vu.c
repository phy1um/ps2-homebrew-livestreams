#include <p2g/log.h>
#include <string.h>

#include "buffer.h"
#include "draw.h"
#include "vu.h"
#include "internal.h"

extern struct render_state state;

int draw_vu_upload_program(void *buf, size_t buf_size, int vu_uprog_addr,
                           int vu_target) {
  trace("vu program upload begin (uprog addr=%d), buffer@=%d", vu_uprog_addr,
        state.buffer.offset);
  struct commandbuffer *c = &state.buffer;
  if (c->vif.is_active) {
    draw_vifcode_end(c);
  }
  size_t dwc = buf_size / 8;
  if (dwc > 256) {
    logerr("vu microprog is too large");
    return 1;
  } else if (dwc == 256) {
    dwc = 0;
  }
  c->vif.is_active = 0;
  command_buffer_align_head(c, 8);
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  vifcode((uint32_t *)c->head, VIF_CODE_MPG, VIF_CODE_NO_STALL, dwc,
          vu_uprog_addr / 8);
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  // do actual transfer
  size_t n_bytes = (buf_size / 8) * 8;
  memcpy(c->head, buf, n_bytes);
  c->head += n_bytes;
  c->offset += n_bytes;
  trace("vu program upload end, buffer@=%d", state.buffer.offset);
  return 0;
}

int draw_vu_call_program(int vu_uprog_addr) {
  trace("vu program call (uprog addr=%d), buffer@=%d", vu_uprog_addr,
        state.buffer.offset);
  struct commandbuffer *c = &state.buffer;
  if (!c->dma.in_cnt) {
    draw_start_cnt(c);
    command_buffer_align_head(c, 16);
  }
  if (c->vif.is_active) {
    draw_vifcode_end(c);
  } 
  trace("vu program call [[ before write ]], buffer@=%d",
        state.buffer.offset);
  vifcode((uint32_t *)c->head, VIF_CODE_MSCAL, VIF_CODE_NO_STALL, 0,
          vu_uprog_addr / 8);
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  return 0;
}

int draw_vu_begin_unpack_verts(int unpack_fmt, uint64_t gif_regs, uint16_t nregs, int vu_addr) {
  struct commandbuffer *c = &state.buffer;
  draw_vifcode_end(c);

  c->head +=  32;
  c->offset += 32;

  int qw_base = (c->offset / 16)*16;
  if (c->offset % 16 > 12) {
    logerr("unhandled alignment case");
    return 1;
  }
  while (c->offset - qw_base != 12) {
    c->head += 1;
    c->offset += 1;
  }
  trace("begin unpack @buffer=%d, vu addr = %lu, GIFTag regs = %llX(%d), fmt = %d",
      c->offset, vu_addr, gif_regs, nregs, unpack_fmt);
  vifcode((uint32_t *)c->head, unpack_fmt, VIF_CODE_NO_STALL, 0,
          vu_addr);
  c->vif.head = c->head;
  c->vif.is_active = 1;
  c->vif.is_direct_gif = 0;
  c->vif.is_unpack = 1;
  c->vif.unpack_byte_sum = 0;
  c->vif.unpack_fmt = unpack_fmt;
  c->vif.unpack_nregs = nregs;

  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);

  command_buffer_align_head(c, 16);
  // TODO: hack
  c->gif.head = 0;
  c->vif.unpack_cnt_working_nloop_offset = c->offset;
  c->vif.unpack_giftag_head = (uint32_t*)(c->head-16);
  trace("vif unpack giftag ptr = %p (%p) (aka %d)", c->vif.unpack_giftag_head, c->head-16, c->offset - 16);
  return 0;
}

