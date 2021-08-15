
#ifndef DRAW_H
#define DRAW_H

#define MESH_HEADER_SIZE 3
#define myftoi4(x) (((uint64_t)(x))<<4)

struct render_state {
  float offset_x;
  float offset_y;
  char clear_col[3];
  MATRIX world_view;
  MATRIX view_screen;
  MATRIX world_to_screen;
  VECTOR camera_pos;
  VECTOR camera_tgt;
  VECTOR up;
};

struct model_instance {
  struct model *m;
  VECTOR translate;
  VECTOR scale;
  VECTOR rotate;
};


void mesh_transform(char *b, struct model_instance *inst, struct render_state *d);
void create_model_matrix(MATRIX tgt, VECTOR translate, VECTOR scale, VECTOR rotate);
void update_draw_matrix(struct render_state *d);

#endif
