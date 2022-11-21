
#ifndef P2G_SRC_GS_STATE_H
#define P2G_SRC_GS_STATE_H

struct gs_state {
  framebuffer_t fb[2];
  zbuffer_t zb;
  int ctx;
};

extern struct gs_state *GS_STATE;

#endif
