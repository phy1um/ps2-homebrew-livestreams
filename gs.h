#ifndef GS_H
#define GS_H

#include <graph.h>
#include <draw.h>

struct draw_state {
  int width;
  int height;
  int vmode;
  int gmode;
  framebuffer_t fb;
  zbuffer_t zb;
};

int gs_init(struct draw_state *ds, int psm, int psmz);

#endif
