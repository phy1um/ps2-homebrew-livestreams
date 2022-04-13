#include <gs_gp.h>

#include "draw.h"
#include "buffer.h"

#include "../log.h"

int giftag_new(struct d2d_state *s, int flag, int nloop, int eop, int nregs,
    uint64_t regs) {
  // update giftag tracking state
  s->gif.loop_count = 0;
  s->gif.head = (uint32_t *) s->drawbuffer_head;
  trace("GIFTag %d @ %p", flag, s->gif.head);

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
  s->drawbuffer_head_offset += QW_SIZE;
  return 1;
}

#define GIF_AD(b, reg, value) do {\
    ((uint64_t*)b)[1] = (reg); \
    ((uint64_t*)b)[0] = (value);\
  }while(0)

#define SHIFT(v, m, p) ((((uint64_t)v) & (m)) << p)

// TODO(phy1um): intline?
int gif_ad(struct d2d_state *s, uint64_t reg, uint64_t value) {
  GIF_AD(s->drawbuffer_head, reg, value);
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_head_offset += QW_SIZE;
  return 1;
}

int giftag_ad_texflush(struct d2d_state *s) {
  gif_ad(s, GS_REG_TEXFLUSH, 0);
  return 1;
}

int giftag_ad_prim(struct d2d_state *s,
  int type, int shaded, int textured, int aa) {
  gif_ad(s, GS_REG_PRIM,
      type
      | SHIFT(shaded, 1, 3)
      | SHIFT(textured, 1, 4)
      | SHIFT(1, 1, 6)  // this one is alpha
      | SHIFT(aa, 1, 7));
  return 1;
}

int giftag_ad_bitbltbuf(struct d2d_state *s, int dba, int dbw, uint64_t psm) {
  gif_ad(s, GS_REG_BITBLTBUF,
      SHIFT(dba, 0x3fff, 32) | SHIFT(dbw, 0x3f, 48) | SHIFT(psm, 0x3f, 56));
  return 1;
}

int giftag_ad_trxpos(struct d2d_state *s, int sx, uint64_t sy, uint64_t dx,
    uint64_t dy, uint64_t dir) {
  gif_ad(s, GS_REG_TRXPOS,
      SHIFT(sx, 0x7ff, 0)
      | SHIFT(sy, 0x7ff, 16)
      | SHIFT(dx, 0x7ff, 32)
      | SHIFT(dy, 0x7ff, 48)
      | SHIFT(dir, 0x3, 59));
  return 1;
}

int giftag_ad_trxdir(struct d2d_state *s, int dir) {
  gif_ad(s, GS_REG_TRXDIR,
      dir&0x3);
  return 1;
}

int giftag_ad_trxreg(struct d2d_state *s, int rrw, int rrh) {
  gif_ad(s, GS_REG_TRXREG,
      SHIFT(rrw, 0xfff, 0) | SHIFT(rrh, 0xfff, 32));
  return 1;
}

// TODO(phy1um): the value is always 0 for some reason
int giftag_ad_texa(struct d2d_state *s, int ta0, int ta1) {
  gif_ad(s, GS_REG_TEXA,
      SHIFT(ta0, 0xff, 0) | SHIFT(ta1, 0xff, 32));
  return 1;
}

int giftag_ad_tex0(struct d2d_state *s,
    int reg, int tbp, int tbw, int psm, int tw, int th, int tcc, int tfx) {
  gif_ad(s, GS_REG_TEX0 + reg,
      SHIFT(tbp, 0x3fff, 0)
      | SHIFT(tbw, 0x3f, 14)
      | SHIFT(psm, 0x3f, 20)
      | SHIFT(tw, 0xf, 26)
      | SHIFT(th, 0xf, 30)
      | SHIFT(tcc, 0x1, 34)
      | SHIFT(tfx, 0x3, 35));
  return 1;
}

int giftag_ad_tex1(struct d2d_state *s, int lcm, int mxl, int mtba,
    int l, int k) {
  gif_ad(s, GS_REG_TEX1,
      SHIFT(lcm, 0x1, 0)
      | SHIFT(mxl, 0x7, 2)
      | SHIFT(mtba, 0x1, 8)
      | SHIFT(l, 0x3, 19)
      | SHIFT(k, 0x7ff, 32));
  return 1;
}

int giftag_ad_tex2(struct d2d_state *s, int psm, int cbp, int cpsm,
    int csm, int csa, int cld) {
  gif_ad(s, GS_REG_TEX2,
      SHIFT(psm, 0x3f, 20)
      | SHIFT(cbp, 0x3fff, 37)
      | SHIFT(csm, 1, 55)
      | SHIFT(csa, 0x1f, 56)
      | SHIFT(cld, 0x7, 61));
  return 1;
}

int giftag_ad_alpha(struct d2d_state *s, int a, int b, int c, int d, int fix) {
  gif_ad(s, GS_REG_ALPHA,
      SHIFT(a, 0x3, 0)
      | SHIFT(b, 0x3, 2)
      | SHIFT(c, 0x3, 4)
      | SHIFT(d, 0x3, 6)
      | SHIFT(fix, 0xff, 32));
  return 1;
}

int push_rgbaq(struct d2d_state *s, unsigned char cols[4]) {
  uint32_t *v = (uint32_t *)s->drawbuffer_head;
  v[0] = cols[0];
  v[1] = cols[1];
  v[2] = cols[2];
  v[3] = cols[3];
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_head_offset += QW_SIZE;
  return 1;
}

int push_xyz2(struct d2d_state *s, uint16_t x, uint16_t y, uint32_t z) {
  uint32_t *v = (uint32_t*)s->drawbuffer_head;
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = 0;
  s->drawbuffer_head += QW_SIZE;
  s->drawbuffer_head_offset += QW_SIZE;
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
  state->drawbuffer_head_offset += QW_SIZE;
  return 1;
}

int dma_tag(uint32_t *t, int qwc, int type, uint32_t addr) {
  trace("DMATag @ %p", t);
  t[0] = (qwc&0xffff) | type;
  t[1] = addr;
  t[2] = 0;
  t[3] = 0;
  return 1;
}

