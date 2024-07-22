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
#include <p2g/ps2math.h>

#include "buffer.h"
#include "draw.h"
#include "internal.h"

#define GIF_REGS_AD 0xe
#define GIF_REGS_AD_LEN 1
#define GIF_REGS_FMT_GEOM3D 0x515151
#define GIF_REGS_FMT_GEOM3D_LEN 6

extern struct render_state state;

#define fp_just_decimal(x) ((x) - ((int)(x)))

#define to_coord(f)                                                            \
  0x8000 + (0xfff0 & (((int)(f) << 4))) + (fp_just_decimal(f) * 0xf)

// prepend instance data for batch drawing
int draw3d_instance_xyz(float x, float y, float z) { return 0; }

int draw3d_instance_rgba(char r, char g, char b, char a) { return 0; }

int draw3d_instance_header_uint32(uint32_t value) { return 0; }

// put a mesh into drawbuffer by reference (DMA copies without CPU copy)
int draw3d_mesh_triangles_ref(void *buffer, int vertex_count,
                              size_t vertex_size) {
  trace("triangle mesh ref @ %u", state.buffer.offset);
  if (state.buffer.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    // wtf do we do here if we have to split
    draw_kick();
  }
  draw_end_cnt(&state.buffer);
  int buffer_size = vertex_count * vertex_size;
  int qwc = buffer_size / 4 + (buffer_size % 4 == 0 ? 0 : 1);
  draw_dma_ref(&state.buffer, (uint32_t)buffer, qwc);
  state.this_frame.tris += vertex_count / 3;
  return 0;
}

// put a mesh into drawbuffer by copy (needed to do CPU transforms)
size_t draw3d_mesh_triangles_cnt(void *buffer, int vertex_count,
                                 size_t vertex_size) {
  if (state.buffer.gif.loop_count >= GIF_MAX_LOOPS - 1) {
    draw_kick();
  }

  int buffer_size = vertex_count * vertex_size;

  trace("triangle mesh cnt @ %u [#vert = %d, |vert| = %d]", state.buffer.offset,
        vertex_count, vertex_size);

  if (state.buffer.offset >= state.buffer.length - buffer_size) {
    logerr("no room to CNT draw model");
    return -1;
  }

  if (state.d2d.draw_type != DRAW_FMT_GEOM3D) {
    if (state.d2d.draw_type != DRAW_FMT_NONE) {
      commandbuffer_update_last_tag_loop(&state.buffer);
    }
    draw_giftags_begin(&state.buffer);
    giftag_new(&state.buffer, 0, 1, 0, GIF_REGS_AD_LEN, GIF_REGS_AD);
    giftag_ad_prim(&state.buffer, GS_PRIM_TRIANGLE, 0, 0, 0);
    giftag_new(&state.buffer, 0, 1, 0, GIF_REGS_FMT_GEOM3D_LEN,
               GIF_REGS_FMT_GEOM3D);
    state.d2d.draw_type = DRAW_FMT_GEOM;
  }

  size_t out = state.buffer.offset;
  memcpy(state.buffer.head, buffer, buffer_size);
  state.buffer.head += buffer_size;
  state.buffer.offset += buffer_size;
  state.this_frame.tris += vertex_count / 3;
  state.buffer.gif.loop_count += vertex_count / 3;
  return out;
}

// apply 4x4 matrix transform to a mesh copied into the drawbuffer
int draw3d_ee_transform_verts(float *mvp, size_t offset_start, int vertex_count,
                              int vertex_size, int pos_offset) {

  trace("EE transform @ %u [#vert = %d, |vert| = %d]", offset_start,
        vertex_count, vertex_size);
  size_t vertex_base = offset_start;
  for (int i = 0; i < vertex_count; i++) {
    float *vertex = (float *)(state.buffer.ptr + vertex_base);
    float *position = vertex + pos_offset;
    position[3] = 1.f;
    trace(" read verts @ %p(%d) [+%d => %p]", vertex, i, pos_offset, position);
    trace(" @ {%f, %f, %f, %f}", position[0], position[1], position[2],
          position[3]);
    p2m_m4_apply(mvp, position);
    trace(" => {%f, %f, %f, %f}", position[0], position[1], position[2],
          position[3]);

    // convert to fixed point from float
    int *position_fp = (int *)position;
    position_fp[0] = to_coord((position[0] / position[3]));
    position_fp[1] = to_coord((position[1] / position[3]));
    position_fp[2] = (2 << 15) - (int)((-2000 * position[2] / position[3]));
    position_fp[3] = 0;
    trace(" => {%x, %x, %x, %x}", position_fp[0], position_fp[1],
          position_fp[2], position_fp[3]);

    vertex_base += vertex_size;
  }
  return vertex_count;
}
