
#ifndef PS2_HOMEBREW_DRAW_DRAW_H
#define PS2_HOMEBREW_DRAW_DRAW_H

/**
 * CONSTANTS
 */

#define GIF_MAX_LOOPS 0x7fff


/**
 * DRAW 2D
 */
struct d2d_gif;
struct d2d_dma;

struct d2d_stats {
  int kick_count;
  int tris;
}

struct d2d_state {
  char col[4]; 
  enum draw_type draw_type;
  int vram_texture_ptr;
  char *drawbuffer;
  char *drawbuffer_head;
  int drawbuffer_len;

  struct d2d_gif gif;
  struct d2d_dma dma;
  struct d2d_stats this_frame;
  struct d2d_stats last_frame;
};


int draw2d_frame_start();
int draw2d_frame_end();
int draw2d_triangle(float x1, float y1, float x2, float y2, float x3, float y3);

#endif
