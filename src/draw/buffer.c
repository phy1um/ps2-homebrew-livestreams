#include <gs_gp.h>

#include "draw.h"
#include "buffer.h"

#include "../log.h"

int giftag_new(struct d2d_state *s, int flag, int nloop, int eop, int nregs,
    uint64_t regs) {
  trace("new giftag %d @ %u", flag, s->drawbuffer_size);
  // update giftag tracking state
  s->gif.loop_count = 0;
  s->gif.head = (uint32_t *) s->drawbuffer_head;

  // write giftag
  uint64_t *ld = (uint64_t*)s->drawbuffer_head;
  uint64_t *ud = ((uint64_t*)(s->drawbuffer_head)) + 1;
  if (nloop > 0x7fff) {
    error("invalid giftag: nloops > 0x7fff");
  }
  *ld = (eop * 0x8000) | nloop;
  *ld |= (((uint64_t)flag)&0x3) << 58;
  *ld |= (((uint64_t)nregs)&0xf) << 60;
  *ud = regs;

  // advance head
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_size += QW_SIZE;
  return 1;
}

#define GIF_AD(b, reg, value) do {\
    ((uint64_t*)b)[1] = (reg); \
    ((uint64_t*)b)[0] = (value);\
  }while(0)

int giftag_ad_prim(struct d2d_state *s,
  int type, int shaded, int textured, int aa) {
  GIF_AD(s->drawbuffer_head,
      GS_REG_PRIM,
      type | (shaded*0x8) | (textured*0x10) | (aa*0x80));
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_size += QW_SIZE;
  return 1;
}

int push_rgbaq(struct d2d_state *s, unsigned char cols[4]) {
  uint32_t *v = (uint32_t *)s->drawbuffer_head;
  v[0] = cols[0];
  v[1] = cols[1];
  v[2] = cols[2];
  v[3] = cols[3];
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_size += QW_SIZE;
  return 1;
}

int push_xyz2(struct d2d_state *s, uint16_t x, uint16_t y, uint32_t z) {
  uint32_t *v = (uint32_t*)s->drawbuffer_head;
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = 0;
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_size += QW_SIZE;
  return 1;
}

// ASSERT sizeof(float) == sizeof(uint32_t)
int push_st(struct d2d_state *state, float s, float t) {
  float *v = (float *)state->drawbuffer_head;
  v[0] = s;
  v[1] = t;
  v[2] = 1.0;
  v[3] = 0;
  state->drawbuffer_head += QW_SIZE;
  state->drawbuffer_size += QW_SIZE;
  return 1;
}

int dma_tag(uint32_t *t, int qwc, int type, uint32_t addr) {
  t[0] = (qwc&0xffff) | type;
  t[1] = addr;
  t[2] = 0;
  t[3] = 0;
  return 1;
}

