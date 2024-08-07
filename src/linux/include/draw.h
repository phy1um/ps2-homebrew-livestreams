
#ifndef P2SIM_DRAW_H
#define P2SIM_DRAW_H

#include <tamtypes.h>
#include <draw_tests.h>
#include <draw_blending.h>
#include <draw_sampling.h>
#include <draw_primitives.h>
#include <gs_gp.h>
#include <gif_tags.h>

#define ftoi4(f) ((int)f)

#define DRAW_XYZ_REGLIST \
  ((u64)GIF_REG_XYZ2) << 0 | \
  ((u64)GIF_REG_XYZ2) << 4

typedef union {
  u64 rgbaq;
  struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    float q;
  };
} __attribute__((packed, aligned(8))) color_t;

typedef struct {
  float x; float y; unsigned int z;
} vertex_t;

typedef struct {
  vertex_t v0;
  vertex_t v1;
  color_t color;
} rect_t;


#define DRAW_DISABLE 0
#define DRAW_ENABLE 1

enum draw_psm {
  GS_PSM_4,
  GS_PSM_4HL,
  GS_PSM_4HH,
  GS_PSM_8,
  GS_PSM_8H,
  GS_PSM_16,
  GS_PSM_16S,
  GS_PSM_24,
  GS_PSM_PS24,
  GS_PSM_32,
  GS_PSMZ_16,
  GS_PSMZ_16S,
  GS_PSMZ_24,
  GS_PSMZ_32,
};

typedef struct {
  int address;
  int width;
  int height;
  int mask;
  enum draw_psm psm;
} framebuffer_t;

typedef struct {
  int enable;
  int address;
  int width;
  int height;
  int method;
  int mask;
  enum draw_psm zsm;
} zbuffer_t;

typedef struct {
  int enable;
  int method;
  int compval;
  int keep;
} atest_t;

typedef struct {
  int enable;
  int pass;
} dtest_t;

typedef struct {
  int enable;
  int method;
} ztest_t;

qword_t * draw_finish(qword_t *q);
void draw_wait_finish();

qword_t *draw_framebuffer(qword_t *q, int context, framebuffer_t *frame);
qword_t *draw_zbuffer(qword_t *q, int context, zbuffer_t *zbuffer);
qword_t *draw_dithering(qword_t *q, int enable);
qword_t *draw_dither_matrix(qword_t *q, char *dm);
qword_t *draw_fog_color(qword_t *q, unsigned char r, unsigned char g,
    unsigned char b);
qword_t *draw_scan_masking(qword_t *q, int mask);
qword_t *draw_color_clamping(qword_t *q, int enable);
qword_t *draw_alpha_correction(qword_t *q, int context, int alpha);
qword_t *draw_primitive_xyoffset(qword_t *q, int context, float x, float y);
qword_t *draw_primitive_override(qword_t *q, int mode);
qword_t *draw_scissor_area(qword_t *q, int context, int x0, int x1, int y0,
    int y1);

qword_t *draw_setup_environment(qword_t *q, int context, framebuffer_t *frame,
    zbuffer_t *z);

qword_t *draw_disable_tests(qword_t *q, int i, zbuffer_t *zb);
qword_t *draw_clear(qword_t *q, int context, float x, float y, float width,
    float height, int r, int g, int b);
qword_t *draw_rect_filled_strips(qword_t *q, int context, rect_t *rect);

#endif
