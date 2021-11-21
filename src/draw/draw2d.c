#include <dma.h>
#include <dma_tags.h>
#include <draw.h>
#include <gs_gp.h>
#include <gs_psm.h>
#include <tamtypes.h>

#include <string.h>
#include <stdlib.h>

#include "draw.h"
#include "buffer.h"
#include "../log.h"

static struct d2d_state state;

int draw2d_alloc() {
  state.drawbuffer = malloc(20000);
  return 1;
}

#define GIF_REGS_AD 0xe
#define GIF_REGS_AD_LEN 1
#define GIF_REGS_TRIS 0x1555
#define GIF_REGS_TRIS_LEN 4
#define GIF_REGS_SPRITE 0x215215
#define GIF_REGS_SPRITE_LEN 6

// easy :D
int draw2d_new_buffer() {
  state.drawbuffer_head = state.drawbuffer;
  return 1;
}


int draw2d_update_last_tag_loops() {
  if (state.gif.loop_count <= GIF_MAX_LOOPS) {
    uint32_t eop = *state.gif.head & 0x8000;
    *state.gif.head = state.gif.loop_count | eop;
  } else {
    error("too many loops in GIFTag");
  }
  return 1;
}

int draw2d_end_cnt() {
  return 1;
}

int draw2d_dma_end() {
  return 1;
}

int draw2d_kick() {
  trace("kick");
  draw2d_update_last_tag_loops();
  draw2d_end_cnt();
  draw2d_dma_end();
  size_t buffer_size = state.drawbuffer_head - state.drawbuffer;
  trace("dma send");
  dma_channel_send_chain(
      DMA_CHANNEL_GIF,
      state.drawbuffer,
      buffer_size / 16,
      0,
      0);
  // TODO(phy1um): get new memory?
  draw2d_new_buffer();
  state.drawbuffer_head = state.drawbuffer;
  state.this_frame.kick_count += 1;
  return 1;
}

#define to_coord(f) \
  (uint16_t) ( ((int)(f) << 4) | (int)( ((f) - ((int)(f))) * 0xf ) )

int draw2d_triangle(float x1, float y1,
    float x2, float y2, float x3, float y3) {
  trace("tri @ %f,%f  %f,%f  %f,%f", x1, y1, x2, y2, x3, y3);
  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw2d_kick();
  }

  if (state.drawbuffer - state.drawbuffer_head >= 20000) {
    draw2d_kick();
  }

  if (state.draw_type != D2D_GEOM) {
    if (state.draw_type != D2D_NONE) {
      draw2d_update_last_tag_loops();
    }

    giftag_new(&state, 0, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_prim(&state, PRIM_TRIANGLE, 0, 0, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_TRIS_LEN, GIF_REGS_TRIS);
    state.draw_type = D2D_GEOM;
  }

  push_rgbaq(&state, state.col);
  push_xyz2(&state, to_coord(x1-320), to_coord(y1-224), 0);
  push_xyz2(&state, to_coord(x2-320), to_coord(y2-224), 0);
  push_xyz2(&state, to_coord(x3-320), to_coord(y3-224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}

static zbuffer_t zb = {0};
int draw2d_frame_start() {
  trace("frame start");
  memset(&state.this_frame, 0, sizeof(struct d2d_stats)); 
  draw2d_new_buffer(); 

  // Clear the screen using PS2SDK functions
  float halfw = (state.screen_w*1.0f) / 2.0f;
  float halfh = (state.screen_h*1.0f) / 2.0f;
  qword_t *q = (qword_t *) state.drawbuffer_head;
  q = draw_disable_tests(q, 0, &zb);
  PACK_GIFTAG(q, GIF_SET_TAG(1, 0, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q,
              GS_SET_TEST(DRAW_ENABLE, ATEST_METHOD_NOTEQUAL, 0x00,
                          ATEST_KEEP_FRAMEBUFFER, DRAW_DISABLE, DRAW_DISABLE,
                          DRAW_ENABLE, ZTEST_METHOD_ALLPASS),
              GS_REG_TEST);
  q++;
  q = draw_clear(q, 0, 2048.0f - halfw, 2048.0f - halfh, 
      state.screen_w, state.screen_h, 
      state.clear[0], state.clear[1], state.clear[2]);
  state.drawbuffer_head = (char *) q;
  return 1;
}

int draw2d_frame_end() {
  trace("frame end");
  qword_t *q = draw_finish((qword_t *) state.drawbuffer_head);
  state.drawbuffer_head = (char *) q;
  draw2d_kick();
  memcpy(&state.last_frame, &state.this_frame, sizeof(struct d2d_stats));
  return 1;
}

int draw2d_set_colour(char r, char g, char b, char a) {
  state.col[0] = r;
  state.col[1] = g;
  state.col[2] = b;
  state.col[3] = a;
  return 1;
}

int draw2d_clear_colour(char r, char g, char b) {
  state.clear[0] = r;
  state.clear[1] = g;
  state.clear[2] = b;
  return 1;
}

int draw2d_screen_dimensions(int w, int h) {
  state.screen_w = w;
  state.screen_h = h;
  return 1;
}

