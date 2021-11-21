#include "draw.h"
#include "buffer.h"

enum draw_type {
  D2D_NONE,
  D2D_GEOM,
  D2D_SPRITE,
};

struct d2d_gif {
  uint32_t *head;
  int loop_count;
};

struct d2d_dma {
  int head;
  int in_cnt;
};

struct d2d_stats {
  int kick_count;
  int tris;
}

static struct d2d_state state;

#define GIF_REGS_AD 0xe
#define GIF_REGS_AD_LEN 1
#define GIF_REGS_TRIS 0x1555
#define GIF_REGS_TRIS_LEN 4
#define GIF_REGS_SPRITE 0x215215
#define GIF_REGS_SPRITE_LEN 6

int draw2d_update_last_tag_loops()
{
  if (state.gif.loop_count <= GIF_MAX_LOOPS) {
    uint32_t eop = *state.gif.head & 0x8000;
    *state.gif.head = state.gif.loop_count | eop;
  } else {
    error("too many loops in GIFTag");
  }
  return 1;
}

int draw2d_end_cnt()
{
  return 1;
}

int draw2d_dma_end()
{
  return 1;
}

int draw2d_kick()
{
  draw2d_update_last_tag_loops();
  draw2d_end_cnt(); 
  draw2d_dma_end();
  size_t buffer_size = state.drawbuffer_head - state.drawbuffer;
  dma_channel_send_chain(
      DMA_CHANNEL_GIF, 
      state.drawbuffer, 
      buffer_size / 16, 
      0, 
      0
  );
  // TODO: get new memory?
  state.drawbuffer_head = state.drawbuffer;
  state.this_frame.kick_count += 1;
}

int draw2d_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw2d_kick();
  }

  if (state.drawbuffer_len - state.drawbuffer_head < 80) {
    draw2d_kick();
  }
  
  if (state.draw_type != D2D_GEOM) {
    if (self.draw_type != D2D_NONE) {
      draw2d_update_last_tag_loops();
    }

    giftag_new(&state, 0, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_prim(&state, PRIM_TRIANGLE, 0, 0, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_TRIS_LEN, GIF_REGS_TRIS);
    state.draw_type = D2D_GEOM;
  }

  push_rgbaq(&state, state.col[0], state.col[1], state.col[2], state.col[3]);
  push_xyz2(&state, to_coord(x1-320), to_coord(y1-224), 0);
  push_xyz2(&state, to_coord(x2-320), to_coord(y2-224), 0);
  push_xyz2(&state, to_coord(x3-320), to_coord(y3-224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}
