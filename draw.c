
#include <math3d.h>
#include <draw_types.h>

#include <stdint.h>

#include "draw.h"
#include "mesh.h"
#include "log.h"
#include "ps2math.h"

#define ZMAX (1024*1024)

void log_matrix(MATRIX m)
{
  printf("Matrix = \n");
  for(int i = 0; i < 4; i++) {
    int b = i*4;
    printf("%.2f %.2f %.2f %.2f\n", m[b], m[b+1], m[b+2], m[b+3]);
  }
}

void mesh_transform(char *b, struct model_instance *inst, struct render_state *d)
{
  MATRIX tmp;
  MATRIX model;
  matrix_unit(model);
  create_model_matrix(model, inst->translate, inst->scale, inst->rotate);
  matrix_unit(tmp);
  matrix_multiply(tmp, model, d->world_to_screen);

  info("Matrix info");
  log_matrix(d->world_to_screen);
  log_matrix(model);
  log_matrix(tmp);
  int stride = inst->m->vertex_size * 16;
  for (int i = 0; i < inst->m->vertex_count; i++) {
    // get address of current vertex data
    float *pos = (float*) (b + (stride*i) + (inst->m->vertex_position_offset*16));
    VECTOR *v = pos;

    vector_apply(v, v, tmp);
    pos[0] = pos[0]/pos[3];
    pos[1] = pos[1]/pos[3];
    pos[2] = pos[2]/pos[3];

    *((uint32_t*)pos) = ftoi4(pos[0]+d->offset_x);
    *((uint32_t*)(pos+1)) = ftoi4(pos[1]+d->offset_y);
    uint32_t zv = (uint32_t) (ZMAX * (pos[2] + 1000.0f) / 1700.0f);
    *((uint32_t*)(pos+2)) = zv;

    info("vert depth = %d", *((uint32_t*)(pos+2)));

    uint32_t * col = (uint32_t*) (b + (stride*i) + (inst->m->vertex_colour_offset*16));
    col[1] = 0x0f;
    col[2] = 0x0f;
    col[0] = (int) (((zv*1.0f)/(ZMAX*1.0f)) * 255.0f);
    col[3] = 0x80;
    /*
    *((uint32_t*)pos) = (short)((pos[0]+1.0f)*d->offset_x);
    *((uint32_t*)(pos+1)) = (short)((pos[1]+1.0f)*d->offset_y);
    *((uint32_t*)(pos+2)) = (unsigned int)((pos[2]+1.0f)*20);
    */

    pos[3] = 0;
  }
}

void create_model_matrix(MATRIX tgt, VECTOR translate, VECTOR scale, VECTOR rotate)
{
  matrix_unit(tgt);
  matrix_rotate(tgt, tgt, rotate);
  matrix_scale(tgt, tgt, scale);
  matrix_translate(tgt, tgt, translate);
}

void update_draw_matrix(struct render_state *d)
{
  d->up[0] = 0;
  d->up[1] = 1.0f;
  d->up[2] = 0;
  d->up[3] = 0;
  create_view_screen(d->view_screen, 3.0f/4.0f, -3.0f, 3.0f, -3.0f, 3.0f, 1.0f, 2000.0f);
  matrix_lookat(d->world_view, d->camera_pos, d->camera_tgt, d->up);
  matrix_multiply(d->world_to_screen, d->world_view, d->view_screen);
}

