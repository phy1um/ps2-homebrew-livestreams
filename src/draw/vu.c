#include <p2g/log.h>
#include <string.h>

#include "draw.h"
#include "vu.h"
#include "buffer.h"

extern struct render_state state;

static int command_buffer_align_head(struct commandbuffer *c, size_t b) {
  while(c->offset % b != 0) {
    c->head += 1;
    c->offset += 1;
  }
  return 0;
}

int draw_vu_upload_program(void *buf, size_t buf_size, int vu_uprog_addr, int vu_target) {
  trace("vu program upload begin (uprog addr=%d), buffer@=%d", vu_uprog_addr, state.buffer.offset);
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
  vifcode((uint32_t*) c->head, VIF_CODE_MPG, VIF_CODE_NO_STALL, dwc, vu_uprog_addr/8);
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  // do actual transfer
  size_t n_bytes = (buf_size/8)*8;
  memcpy(c->head, buf, n_bytes);
  c->head += n_bytes;
  c->offset += n_bytes;
  trace("vu program upload end, buffer@=%d", state.buffer.offset);
  return 0;
}


int draw_vu_call_program(int vu_uprog_addr) {
  trace("vu program call (uprog addr=%d), buffer@=%d", vu_uprog_addr, state.buffer.offset);
  struct commandbuffer *c = &state.buffer;
  if (c->vif.is_active) {
    draw_vifcode_end(c);
  }
  vifcode((uint32_t*) c->head, VIF_CODE_MSCAL, VIF_CODE_NO_STALL, 0, vu_uprog_addr/8);
  // TODO: hack
  // draw_vifcode_direct_start(&state.buffer);
  return 0;
}

// TODO: support extra flags
int draw_vu_unpack_v4_32(void *buffer, size_t buffer_size, int vu_addr) {
  struct commandbuffer *c = &state.buffer;
  draw_vifcode_end(c);
  size_t qword_size = buffer_size/16; 
  vifcode((uint32_t*) c->head, VIF_CODE_UNPACK_V432, VIF_CODE_NO_STALL, qword_size, vu_addr);

  size_t n_bytes = (qword_size)*16;
  memcpy(c->head, buffer, n_bytes);
  c->head += n_bytes;
  c->offset += n_bytes;

  // TODO: hack
  // draw_vifcode_direct_start(&state.buffer);
  return 0;
}

int draw_vu_begin_unpack_inline(uint32_t target_addr) {
  struct commandbuffer *c = &state.buffer;
  // TODO: this puts a garbage empty DIRECT tag in i think
  // and also alignment is off by 4bytes :)
  draw_vifcode_end(c);
  trace("vu begin inline unpack (vu addr=%d) @buffer=%d", target_addr, c->offset);
  while (c->offset % 12 != 0) {
    c->head += 1;
    c->offset += 1;
  }
  vifcode((uint32_t*) c->head, VIF_CODE_UNPACK_V432, VIF_CODE_NO_STALL, 0, target_addr);
  c->vif.head = c->head;
  c->vif.is_direct_gif = 0;
  c->vif.is_inline_unpack = 1;
  c->vif.is_active = 1;
  c->head += sizeof(uint32_t);
  c->offset += sizeof(uint32_t);
  return 0;
}

