
#ifndef PS2_HOMEBREW_DRAW_BUFFER_H
#define PS2_HOMEBREW_DRAW_BUFFER_H

int giftag_new(struct draw_state *s, int flag, int nloop, int eop, int nregs, uint64_t regs);
int giftag_ad_prim(struct draw_state *s, int, int, int, int);

int push_rgbaq(struct draw_state *s, char cols[4]);
int push_xyz2(struct draw_state *s, uint16_t x, uint16_t y, uint32_t z);
int push_st(struct draw_state *s, float s, float t);

#endif
