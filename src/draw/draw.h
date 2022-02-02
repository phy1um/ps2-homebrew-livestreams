
#ifndef PS2_HOMEBREW_DRAW_DRAW_H
#define PS2_HOMEBREW_DRAW_DRAW_H

#include <stdint.h>
#include <stddef.h>
#include <draw.h>

/**
 * CONSTANTS
 */

#define GIF_MAX_LOOPS 0x7fff
#define QW_SIZE 4*sizeof(uint32_t)

void core_error(const char *);
#define error(msg) core_error(msg)

/**
 * DRAW 2D
 */
enum draw_type {
  D2D_NONE,
  D2D_GEOM,
  D2D_RECT,
  D2D_SPRITE,
};

struct d2d_gif {
  uint32_t *head;
  int loop_count;
};

struct d2d_dma {
  char *head;
  int in_cnt;
};

struct d2d_stats {
  int kick_count;
  int tris;
};

struct d2d_state {
  unsigned char col[4];
  char clear[4];
  int screen_w;
  int screen_h;
  enum draw_type draw_type;
  int vram_texture_ptr;
  char *drawbuffer;
  char *drawbuffer_head;
  size_t drawbuffer_head_offset;
  size_t drawbuffer_len;
  void *zbuffer_ref;

  int tex_vram_addr;
  int tex_width;
  int tex_height;
  int tex_psm;
  int active_tex;

  int clut_tex;

  struct d2d_gif gif;
  struct d2d_dma dma;
  struct d2d_stats this_frame;
  struct d2d_stats last_frame;
};

int draw2d_frame_start();
int draw2d_frame_end();
int draw2d_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
int draw2d_rect(float x1, float y1, float w, float h);

int draw2d_screen_dimensions(int w, int h);
int draw2d_clear_colour(char r, char g, char b);
int draw2d_set_colour(unsigned char r, unsigned char g, unsigned char b,
    unsigned char a);
int draw2d_alloc();

int draw2d_upload_texture(void *texture, size_t bytes, int width, int height,
    int format, int vram_addr);

int draw2d_sprite(float x, float y, float w, float h, float u1, float v1,
    float u2, float v2);

int draw2d_bind_texture(int tex_vram_addr, int width, int height, int psm);

int draw2d_set_clut_state(int texture_base);

int draw2d_bind_buffer(void *buf, size_t buf_len);


#endif
