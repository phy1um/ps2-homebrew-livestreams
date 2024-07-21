
#ifndef PS2_HOMEBREW_DRAW_BUFFER_H
#define PS2_HOMEBREW_DRAW_BUFFER_H

#include "draw.h"

int giftag_new(struct commandbuffer *s, int flag, int nloop, int eop, int nregs,
    uint64_t regs);
int giftag_ad_prim(struct commandbuffer *s, int, int, int, int);
int giftag_ad_texflush(struct commandbuffer *s);
int giftag_ad_bitbltbuf(struct commandbuffer *s, int dba, int dbw, uint64_t psm);
int giftag_ad_trxpos(struct commandbuffer *s, int sx, uint64_t sy, uint64_t dx,
    uint64_t dy, uint64_t dir);
int giftag_ad_trxdir(struct commandbuffer *s, int dir);
int giftag_ad_trxreg(struct commandbuffer *s, int rrw, int rrh);
int giftag_ad_texa(struct commandbuffer *s, int ta0, int ta1);
int giftag_ad_tex0(struct commandbuffer *s,
    int reg, int tbp, int tbw, int psm, int tw, int th, int tcc, int tfx);
int giftag_ad_tex1(struct commandbuffer *s, int lcm, int mxl, int mtba,
    int l, int k);
int giftag_ad_tex2(struct commandbuffer *s, int psm, int cbp, int cpsm,
    int csm, int csa, int cld);
int giftag_ad_alpha(struct commandbuffer *s, int a, int b, int c, int d,
    int fix);


int push_rgbaq(struct commandbuffer *s, unsigned char cols[4]);
int push_xyz2(struct commandbuffer *s, uint16_t x, uint16_t y, uint32_t z);
int push_st(struct commandbuffer *state, float s, float t);

int dma_tag(uint32_t *t, int qwc, int type, uint32_t addr);
int vifcode(uint32_t *t, int op, int stall, int num, uint16_t imm);
int vifcode_update_imm(uint16_t *t, uint16_t imm);

#endif
