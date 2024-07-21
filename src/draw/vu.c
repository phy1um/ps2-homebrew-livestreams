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
  struct commandbuffer *c = &state.buffer;
  if (c->vif.is_active) {
    // TODO: add more general vidcode_end function
    if (c->vif.is_direct_gif) {
      draw_vifcode_end(c);
    }
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
  // TODO: hack
  draw_vifcode_direct_start(&state.buffer);
  return 0;
}


int draw_vu_call_program(int vu_uprog_addr) {
  struct commandbuffer *c = &state.buffer;
  if (c->vif.is_active) {
    // TODO: add more general vidcode_end function
    if (c->vif.is_direct_gif) {
      draw_vifcode_end(c);
    }
  }
  vifcode((uint32_t*) c->head, VIF_CODE_MSCAL, VIF_CODE_NO_STALL, 0, vu_uprog_addr/8);
  // TODO: hack
  draw_vifcode_direct_start(&state.buffer);
  return 0;
}

