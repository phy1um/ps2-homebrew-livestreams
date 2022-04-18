#include <draw.h>

// Normal offset
#define OFFSET 2048.0f

// Leftmost/Topmost offset (2048.0f - 0.4375f + 1)
#define START_OFFSET 2047.5625f

// Bottommost/Rightmost offset (2048.0f + 0.5625f + 1)
#define END_OFFSET 2048.5625f

static char blending = 0;

qword_t *draw_clear(qword_t *q, int context, float x, float y, float width,
                    float height, int r, int g, int b) {

  rect_t rect;

  union {
    float fvalue;
    u32 ivalue;
  } q0 = {1.0f};

  rect.v0.x = x;
  rect.v0.y = y;
  rect.v0.z = 0x00000000;

  rect.color.rgbaq = GS_SET_RGBAQ(r, g, b, 0x80, q0.ivalue);

  rect.v1.x = x + width - 0.9375f;
  rect.v1.y = y + height - 0.9375f;
  rect.v1.z = 0x00000000;

  PACK_GIFTAG(q, GIF_SET_TAG(2, 0, 0, 0, 0, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_ENABLE), GS_REG_PRMODECONT);
  q++;
  PACK_GIFTAG(q, GS_SET_PRMODE(0, 0, 0, 0, 0, 0, context, 1), GS_REG_PRMODE);
  q++;

  q = draw_rect_filled_strips(q, context, &rect);

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, 0, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_DISABLE), GS_REG_PRMODECONT);
  q++;

  return q;
}

qword_t *draw_rect_filled_strips(qword_t *q, int context, rect_t *rect) {

  qword_t *giftag;

  int __xi0 = ftoi4(rect->v0.x);
  int __yi0 = ftoi4(rect->v0.y + START_OFFSET);

  int __xi1 = ftoi4(rect->v1.x);
  int __yi1 = ftoi4(rect->v1.y + END_OFFSET);

  // Start primitive
  PACK_GIFTAG(q, GIF_SET_TAG(2, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;

  PACK_GIFTAG(q, GIF_SET_PRIM(PRIM_SPRITE, 0, 0, 0, blending, 0, 0, context, 0),
              GIF_REG_PRIM);
  q++;

  PACK_GIFTAG(q, rect->color.rgbaq, GIF_REG_RGBAQ);
  q++;

  giftag = q;
  q++;

  // Fill vertex information in 32 pixel wide strips
  while (__xi0 < __xi1) {

    // q->dw[0] = GIF_SET_XYZ(ftoi4(__xf0 + START_OFFSET),__yi0,rect->v0.z);
    q->dw[0] = GIF_SET_XYZ(__xi0 + ftoi4(START_OFFSET), __yi0, rect->v0.z);

    // 31<<4
    __xi0 += 496;

    // Uneven...
    if (__xi0 >= __xi1) {
      __xi0 = __xi1;
    }

    q->dw[1] = GIF_SET_XYZ(__xi0 + ftoi4(END_OFFSET), __yi1, rect->v0.z);

    // 1<<4
    __xi0 += 16;

    q++;
  }

  PACK_GIFTAG(giftag, GIF_SET_TAG(q - giftag - 1, 0, 0, 0, GIF_FLG_REGLIST, 2),
              DRAW_XYZ_REGLIST);

  return q;
}

qword_t *draw_disable_tests(qword_t *q, int context, zbuffer_t *z) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, ZTEST_METHOD_ALLPASS),
              GS_REG_TEST + context);
  q++;

  return q;
}

qword_t *draw_enable_tests(qword_t *q, int context, zbuffer_t *z) {

  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, z->method),
              GS_REG_TEST + context);
  q++;

  return q;
}
