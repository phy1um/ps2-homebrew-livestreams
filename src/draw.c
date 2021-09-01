
#include <draw_types.h>
#include <math3d.h>

#include <stdint.h>

#include "ps2draw.h"
#include "log.h"
#include "mesh.h"
#include "ps2math.h"

#define ZMAX (1024 * 1024)

static int cc = 0;

void log_matrix(MATRIX m) {
  printf("Matrix = \n");
  printf("%.2f %.2f %.2f %.2f\n", m[0], m[4], m[8],  m[12]);
  printf("%.2f %.2f %.2f %.2f\n", m[1], m[5], m[9],  m[13]);
  printf("%.2f %.2f %.2f %.2f\n", m[2], m[6], m[10], m[14]);
  printf("%.2f %.2f %.2f %.2f\n", m[3], m[7], m[11], m[15]);
}

int mesh_is_visible(struct model_instance *inst, struct render_state *d) {
  VECTOR v;
  vector_sub(v, d->camera_pos, inst->translate);
  float dot = v[0]*d->fwd[0] + v[1]*d->fwd[1] + v[2]*d->fwd[2];
  return (dot > 0);
}

void mesh_transform(char *b, struct model_instance *inst,
                    struct render_state *d) {
  MATRIX tmp;
  MATRIX model;
  matrix_unit(model);
  create_model_matrix(model, inst->translate, inst->scale, inst->rotate);
  matrix_unit(tmp);

  matrix_multiply(tmp, tmp, model);
  matrix_multiply(tmp, tmp, d->v);
  matrix_multiply(tmp, tmp, d->p);

  if ( cc == 0 ) {
    info("PROJECTION == ");
    log_matrix(d->p);
  }

  if (cc % 200 == 0) {
    info("###### Matrix info ######");
    info("rotate=%f", d->camera_rotate_y);
    info(" view=");
    log_matrix(d->v);
    info(" mvp=");
    log_matrix(tmp);
  }
  int stride = inst->m->vertex_size * 16;
  float d_avg = 0;
  for (int i = 0; i < inst->m->vertex_count; i++) {
    // get address of current vertex data
    float *pos =
        (float *)(b + (stride * i) + (inst->m->vertex_position_offset * 16));
    float *v = pos;
    pos[3] = 1.f;

    vector_apply(v, v, tmp);
    pos[0] = pos[0]/pos[3]; 
    pos[1] = pos[1]/pos[3];
    pos[2] = pos[2];
    d_avg += pos[2];

    pos[0] = (pos[0]*200) ;
    pos[1] = (pos[1]*200) ;

    *((uint32_t *)pos) = ftoi4(pos[0] + d->offset_x);
    *((uint32_t *)(pos + 1)) = ftoi4(pos[1] + d->offset_y);
    uint32_t zv = (uint32_t)(ZMAX * (pos[2] / 100.f));
    *((uint32_t *)(pos + 2)) = zv;

    uint32_t *col =
        (uint32_t *)(b + (stride * i) + (inst->m->vertex_colour_offset * 16));
    col[1] = 0x0f;
    col[2] = 0x0f;
    col[0] = (int)(((zv) / (ZMAX * 1.0f)) * 255.0f);
    col[3] = 0x80;
    /*
     *((uint32_t*)pos) = (short)((pos[0]+1.0f)*d->offset_x);
     *((uint32_t*)(pos+1)) = (short)((pos[1]+1.0f)*d->offset_y);
     *((uint32_t*)(pos+2)) = (unsigned int)((pos[2]+1.0f)*20);
     */

    pos[3] = 0;
  }
  if ( cc % 100 == 0 ) {
    info("avg depth = %f", d_avg / (1.0f * inst->m->vertex_count));
  }
  cc++;
}

void create_model_matrix(MATRIX tgt, VECTOR translate, VECTOR scale,
                         VECTOR rotate) {
  matrix_unit(tgt);
  matrix_rotate(tgt, tgt, rotate);
  matrix_scale(tgt, tgt, scale);
  matrix_translate(tgt, tgt, translate);
}

void update_draw_matrix(struct render_state *d) {
  d->up[0] = 0;
  d->up[1] = 1.0f;
  d->up[2] = 0;
  d->up[3] = 0;

  d->fwd[0] = 0; d->fwd[1] = 0; d->fwd[2] = -1.f; d->fwd[3] = 1.f;
  vector_rotate_y(d->fwd, d->camera_rotate_y);

  d->camera_tgt[0] = d->camera_pos[0] + d->fwd[0];
  d->camera_tgt[1] = d->camera_pos[1] + d->fwd[1];
  d->camera_tgt[2] = d->camera_pos[2] + d->fwd[2];
  /*
  d->camera_tgt[0] = 0;
  d->camera_tgt[1] = 0;
  d->camera_tgt[2] = 0;
  */
  d->camera_tgt[3] = 1;

  matrix_proj(d->p, 1.2f, 0.7f, .1f, 100.f);
  matrix_lookat(d->v, d->camera_pos, d->camera_tgt, d->up);
}
