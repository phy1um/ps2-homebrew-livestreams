#include <dma.h>
#include <dma_tags.h>
#include <draw_tests.h>
#include <gif_tags.h>
#include <gs_gp.h>
#include <gs_psm.h>
#include <tamtypes.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <p2g/log.h>

#include "buffer.h"
#include "draw.h"
#include "internal.h"

#define GIF_REGS_AD 0xe
#define GIF_REGS_AD_LEN 1
#define GIF_REGS_TRIS 0x5551
#define GIF_REGS_TRIS_LEN 4
#define GIF_REGS_SPRITE 0x512512
#define GIF_REGS_SPRITE_LEN 6
#define GIF_REGS_RECT 0x551
#define GIF_REGS_RECT_LEN 3
#define GIF_REGS_TEXTRI 0x5252521
#define GIF_REGS_TEXTRI_LEN 7

#define BLOCK_SIZE_BYTES 4496

#define GIF_PACKED 0x0
#define GIF_REGLIST 0x1
#define GIF_IMAGE 0x2

#define floorlog2(x) (int)(log2(x))

extern struct render_state state;

#define to_coord(f)                                                            \
  0x8000 + (0xfff0 & (((int)(f) << 4) | (int)((f) - ((int)(f))) * 0xf))

int draw2d_triangle(float x1, float y1, float x2, float y2, float x3,
                    float y3) {
  trace("tri @ %u %f,%f  %f,%f  %f,%f", state.cmdbuffer_head_offset, x1, y1, x2,
        y2, x3, y3);
  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw_kick();
  }

  if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 80) {
    trace("triangle: early kick because buffer is full");
    draw_kick();
  }

  if (state.d2d.draw_type != DRAW_FMT_GEOM) {
    if (state.d2d.draw_type != DRAW_FMT_NONE) {
      draw_update_last_tag_loops();
    }

    giftag_new(&state, 0, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_prim(&state, GS_PRIM_TRIANGLE, 0, 0, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_TRIS_LEN, GIF_REGS_TRIS);
    state.d2d.draw_type = DRAW_FMT_GEOM;
  }

  push_rgbaq(&state, state.d2d.col);
  push_xyz2(&state, to_coord(x1 - 320), to_coord(y1 - 224), 0);
  push_xyz2(&state, to_coord(x2 - 320), to_coord(y2 - 224), 0);
  push_xyz2(&state, to_coord(x3 - 320), to_coord(y3 - 224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}

int draw2d_textri(float x1, float y1, float u1, float v1, float x2, float y2,
                  float u2, float v2, float x3, float y3, float u3, float v3) {
  trace("drawing textri @ %f %f %f %f -- %f %f %f %f -- %f %f %f %f", x1, y1,
        u1, v1, x2, y2, u2, v2, x3, y3, u3, v3);
  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw_kick();
  }

  trace("textri: head = %d, len = %d", state.cmdbuffer_head_offset,
        state.cmdbuffer_len);
  if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 80) {
    trace("textri: early kick because buffer is full");
    draw_kick();
  }

  if (state.d2d.draw_type != DRAW_FMT_TEXTRI) {
    if (state.d2d.draw_type != DRAW_FMT_NONE) {
      draw_update_last_tag_loops();
    }

    giftag_new(&state, 0, 5, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_texa(&state, 0x80, 0x80);
    giftag_ad_tex1(&state, 1, 0, 1, 0, 0);
    giftag_ad_tex0(&state, 0, state.tex_vram_addr / 64, state.tex_width / 64,
                   state.tex_psm, floorlog2(state.tex_width),
                   floorlog2(state.tex_height), 1, 0);
    giftag_ad_tex2(&state, state.tex_psm, state.clut_tex, 0, 0, 0, 0x2);
    giftag_ad_prim(&state, GS_PRIM_TRIANGLE, 0, 1, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_TEXTRI_LEN, GIF_REGS_TEXTRI);
    state.d2d.draw_type = DRAW_FMT_TEXTRI;
  }

  push_rgbaq(&state, state.d2d.col);
  push_st(&state, u1, v1);
  push_xyz2(&state, to_coord(x1 - 320), to_coord(y1 - 224), 0);
  push_st(&state, u2, v2);
  push_xyz2(&state, to_coord(x2 - 320), to_coord(y2 - 224), 0);
  push_st(&state, u3, v3);
  push_xyz2(&state, to_coord(x3 - 320), to_coord(y3 - 224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}

int draw2d_rect(float x1, float y1, float w, float h) {
  trace("rect @ %u %f %f %f %f", state.cmdbuffer_head_offset, x1, y1, w, h);

  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw_kick();
  }

  if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 80) {
    trace("rect: early kick because buffer is full");
    draw_kick();
  }

  if (state.d2d.draw_type != DRAW_FMT_RECT) {
    if (state.d2d.draw_type != DRAW_FMT_NONE) {
      draw_update_last_tag_loops();
    }

    giftag_new(&state, 0, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_prim(&state, GS_PRIM_SPRITE, 0, 0, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_RECT_LEN, GIF_REGS_RECT);
    state.d2d.draw_type = DRAW_FMT_RECT;
  }

  push_rgbaq(&state, state.d2d.col);
  push_xyz2(&state, to_coord(x1 - 320), to_coord(y1 - 224), 0);
  push_xyz2(&state, to_coord(x1 + w - 320), to_coord(y1 + h - 224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}

int draw2d_set_colour(unsigned char r, unsigned char g, unsigned char b,
                      unsigned char a) {
  state.d2d.col[0] = r;
  state.d2d.col[1] = g;
  state.d2d.col[2] = b;
  state.d2d.col[3] = a;
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

int draw_upload_texture(void *texture, size_t bytes, int width, int height,
                        int format, int vram_addr) {
  trace("uploading tex %p -> %d", texture, vram_addr);

  draw_update_last_tag_loops();
  // setup
  giftag_new(&state, 0, 4, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
  giftag_ad_bitbltbuf(&state, vram_addr / 64, width / 64, format);
  giftag_ad_trxpos(&state, 0, 0, 0, 0, 0);
  giftag_ad_trxreg(&state, width, height);
  giftag_ad_trxdir(&state, 0);

  // end current drawing
  draw_end_cnt();

  // assume format == PSM32
  int ee_vram_size = width * height * 4;
  if (ee_vram_size > bytes) {
    logerr("texture upload overflows buffer - bad dimensions %d, %d", width,
           height);
    return 0;
  }

  int qwc = ee_vram_size / 16;
  if (ee_vram_size % 16 != 0) {
    qwc += 1;
  }
  int block_size = BLOCK_SIZE_BYTES / 16;
  int packet_count = qwc / block_size;
  int remain = qwc % block_size;

  trace("upload texture: block_size=%d, packet_count=%d, remain=%d", block_size,
        packet_count, remain);

  // split upload into reasonably sized blocks
  int img_addr = (int)texture;
  while (packet_count > 0) {
    if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 5 * QW_SIZE) {
      trace("upload texture: early kick because buffer is full");
      draw_kick();
      draw_end_cnt();
    }
    dmatag_raw(1, 0x1 << 28, 0);
    giftag_new(&state, GIF_IMAGE, block_size, 0, 0, 0);
    dmatag_raw(block_size, 0x3 << 28, img_addr);

    img_addr += block_size * 16;
    packet_count -= 1;
  }

  // if there is any leftover, handle that here
  if (remain > 0) {
    if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 5 * QW_SIZE) {
      trace("upload texture (remain): early kick because buffer is full");
      draw_kick();
      draw_end_cnt();
    }
    dmatag_raw(1, 0x1 << 28, 0);
    giftag_new(&state, GIF_IMAGE, remain, 0, 0, 0);
    dmatag_raw(remain, 0x3 << 28, img_addr);
  }

  draw_start_cnt();

  giftag_new(&state, GIF_PACKED, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
  giftag_ad_texflush(&state);

  return 1;
}

int draw2d_bind_texture(int tex_vram_addr, int width, int height, int psm) {
  trace("binding texture addr=%d", tex_vram_addr);
  state.tex_vram_addr = tex_vram_addr;
  state.tex_width = width;
  state.tex_height = height;
  state.tex_psm = psm;
  return 1;
}

int draw2d_sprite(float x, float y, float w, float h, float u1, float v1,
                  float u2, float v2) {
  trace("drawing sprite @ %f,%f", x, y);
  if (state.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw_kick();
  }

  if (state.cmdbuffer_head_offset >= state.cmdbuffer_len - 80) {
    trace("sprite: early kick because buffer is full");
    draw_kick();
  }

  if (state.d2d.draw_type != DRAW_FMT_SPRITE ||
      state.active_tex != state.tex_vram_addr) {
    if (state.d2d.draw_type != DRAW_FMT_NONE) {
      draw_update_last_tag_loops();
    }

    state.active_tex = state.tex_vram_addr;
    giftag_new(&state, 0, 5, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_texa(&state, 0x80, 0x80);
    giftag_ad_tex1(&state, 1, 0, 1, 0, 0);
    giftag_ad_tex0(&state, 0, state.tex_vram_addr / 64, state.tex_width / 64,
                   state.tex_psm, floorlog2(state.tex_width),
                   floorlog2(state.tex_height), 1, 0);
    giftag_ad_tex2(&state, state.tex_psm, state.clut_tex, 0, 0, 0, 0x2);
    giftag_ad_prim(&state, GS_PRIM_SPRITE, 0, 1, 0);
    giftag_new(&state, 0, 1, 0, GIF_REGS_SPRITE_LEN, GIF_REGS_SPRITE);
    state.d2d.draw_type = DRAW_FMT_SPRITE;
  }

  push_st(&state, u1, v1);
  push_rgbaq(&state, state.d2d.col);
  push_xyz2(&state, to_coord(x - 320), to_coord(y - 224), 0);
  push_st(&state, u2, v2);
  push_rgbaq(&state, state.d2d.col);
  push_xyz2(&state, to_coord(x + w - 320), to_coord(y + h - 224), 0);
  state.gif.loop_count += 1;
  state.this_frame.tris += 1;
  return 1;
}

int draw2d_set_clut_state(int texture_base) {
  state.clut_tex = texture_base / 64;
  return 1;
}

int draw_bind_buffer(void *buf, size_t buf_len) {
  logdbg("bind buffer %p", buf);
  state.cmdbuffer = buf;
  state.cmdbuffer_len = buf_len;
  return draw_clear_buffer();
}
