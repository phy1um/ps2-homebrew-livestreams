#include <tamtypes.h>

#include <gif_tags.h>
#include <gs_gp.h>

#include <draw.h>

qword_t *draw_setup_environment(qword_t *q, int context, framebuffer_t *frame,
                                zbuffer_t *z) {

  // Change this if modifying the gif packet after the giftag.
  int qword_count = 15;

  atest_t atest;
  dtest_t dtest;
  ztest_t ztest;
  blend_t blend;
  texwrap_t wrap;

  atest.enable = DRAW_ENABLE;
  atest.method = ATEST_METHOD_NOTEQUAL;
  atest.compval = 0x00;
  atest.keep = ATEST_KEEP_FRAMEBUFFER;

  dtest.enable = DRAW_DISABLE;
  dtest.pass = DRAW_DISABLE;

  // Enable or Disable ZBuffer
  if (z->enable) {
    ztest.enable = DRAW_ENABLE;
    ztest.method = z->method;
  } else {
    z->mask = 1;
    ztest.enable = DRAW_ENABLE;
    ztest.method = ZTEST_METHOD_ALLPASS;
  }

  // Setup alpha blending
  blend.color1 = BLEND_COLOR_SOURCE;
  blend.color2 = BLEND_COLOR_DEST;
  blend.alpha = BLEND_ALPHA_SOURCE;
  blend.color3 = BLEND_COLOR_DEST;
  blend.fixed_alpha = 0x80;

  // Setup whole texture clamping
  wrap.horizontal = WRAP_CLAMP;
  wrap.vertical = WRAP_CLAMP;
  wrap.minu = wrap.maxu = 0;
  wrap.minv = wrap.maxv = 0;

  // Begin packed gif data packet with another qword.
  PACK_GIFTAG(q, GIF_SET_TAG(qword_count, 0, 0, 0, GIF_FLG_PACKED, 1),
              GIF_REG_AD);
  q++;
  // Framebuffer setting
  PACK_GIFTAG(q,
              GS_SET_FRAME(frame->address >> 11, frame->width >> 6, frame->psm,
                           frame->mask),
              GS_REG_FRAME + context);
  q++;
  // ZBuffer setting
  PACK_GIFTAG(q, GS_SET_ZBUF(z->address >> 11, z->zsm, z->mask),
              GS_REG_ZBUF + context);
  q++;
  // Override Primitive Control
  PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_DISABLE), GS_REG_PRMODECONT);
  q++;
  // Primitive coordinate offsets
  PACK_GIFTAG(q, GS_SET_XYOFFSET(ftoi4(2048.0f), ftoi4(2048.0f)),
              GS_REG_XYOFFSET + context);
  q++;
  // Scissoring area
  PACK_GIFTAG(q, GS_SET_SCISSOR(0, frame->width - 1, 0, frame->height - 1),
              GS_REG_SCISSOR + context);
  q++;
  // Pixel testing
  PACK_GIFTAG(q,
              GS_SET_TEST(atest.enable, atest.method, atest.compval, atest.keep,
                          dtest.enable, dtest.pass, ztest.enable, ztest.method),
              GS_REG_TEST + context);
  q++;
  // Fog Color
  PACK_GIFTAG(q, GS_SET_FOGCOL(0, 0, 0), GS_REG_FOGCOL);
  q++;
  // Per-pixel Alpha Blending (Blends if MSB of ALPHA is true)
  PACK_GIFTAG(q, GS_SET_PABE(DRAW_DISABLE), GS_REG_PABE);
  q++;
  // Alpha Blending
  PACK_GIFTAG(q,
              GS_SET_ALPHA(blend.color1, blend.color2, blend.alpha,
                           blend.color3, blend.fixed_alpha),
              GS_REG_ALPHA + context);
  q++;
  // Dithering
  PACK_GIFTAG(q, GS_SET_DTHE(GS_DISABLE), GS_REG_DTHE);
  q++;
  PACK_GIFTAG(q, GS_SET_DIMX(4, 2, 5, 3, 0, 6, 1, 7, 5, 3, 4, 2, 1, 7, 0, 6),
              GS_REG_DIMX);
  q++;
  // Color Clamp
  PACK_GIFTAG(q, GS_SET_COLCLAMP(GS_ENABLE), GS_REG_COLCLAMP);
  q++;
  // Alpha Correction
  if ((frame->psm == GS_PSM_16) || (frame->psm == GS_PSM_16S)) {
    PACK_GIFTAG(q, GS_SET_FBA(ALPHA_CORRECT_RGBA16), GS_REG_FBA + context);
    q++;
  } else {
    PACK_GIFTAG(q, GS_SET_FBA(ALPHA_CORRECT_RGBA32), GS_REG_FBA + context);
    q++;
  }
  // Texture wrapping/clamping
  PACK_GIFTAG(q,
              GS_SET_CLAMP(wrap.horizontal, wrap.vertical, wrap.minu, wrap.maxu,
                           wrap.minv, wrap.maxv),
              GS_REG_CLAMP + context);
  q++;
  PACK_GIFTAG(q, GS_SET_TEXA(0x80, ALPHA_EXPAND_NORMAL, 0x80), GS_REG_TEXA);
  q++;

  return q;
}

qword_t *draw_framebuffer(qword_t *q, int context, framebuffer_t *frame) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_FRAME(frame->address >> 11, frame->width >> 6, frame->psm,
                           frame->mask),
              GS_REG_FRAME + context);
  q++;

  return q;
}

qword_t *draw_zbuffer(qword_t *q, int context, zbuffer_t *zbuffer) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_ZBUF(zbuffer->address >> 11, zbuffer->zsm, zbuffer->mask),
              GS_REG_ZBUF + context);
  q++;

  return q;
}

qword_t *draw_dithering(qword_t *q, int enable) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_DTHE(enable), GS_REG_DTHE);
  q++;

  return q;
}

qword_t *draw_dither_matrix(qword_t *q, char *dm) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q,
              GS_SET_DIMX(dm[0], dm[1], dm[2], dm[3], dm[4], dm[5], dm[6],
                          dm[7], dm[8], dm[9], dm[10], dm[11], dm[12], dm[13],
                          dm[14], dm[15]),
              GS_REG_DIMX);
  q++;

  return q;
}

qword_t *draw_fog_color(qword_t *q, unsigned char r, unsigned char g,
                        unsigned char b) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_FOGCOL(r, g, b), GS_REG_FOGCOL);
  q++;

  return q;
}

qword_t *draw_scan_masking(qword_t *q, int mask) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_SCANMSK(mask), GS_REG_SCANMSK);
  q++;

  return q;
}

qword_t *draw_color_clamping(qword_t *q, int enable) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_COLCLAMP(enable), GS_REG_COLCLAMP);
  q++;

  return q;
}

qword_t *draw_alpha_correction(qword_t *q, int context, int alpha) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_FBA(alpha), GS_REG_FBA + context);
  q++;

  return q;
}

qword_t *draw_primitive_xyoffset(qword_t *q, int context, float x, float y) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_XYOFFSET((int)(x * 16.0f), (int)(y * 16.0f)),
              GS_REG_XYOFFSET + context);
  q++;

  return q;
}

qword_t *draw_primitive_override(qword_t *q, int mode) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_PRMODECONT(mode), GS_REG_PRMODECONT);
  q++;

  return q;
}

qword_t *draw_scissor_area(qword_t *q, int context, int x0, int x1, int y0,
                           int y1) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GS_SET_SCISSOR(x0, x1, y0, y1), GS_REG_SCISSOR + context);
  q++;

  return q;
}
