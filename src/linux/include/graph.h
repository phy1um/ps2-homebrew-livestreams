
#ifndef P2SIM_GRAPH_H
#define P2SIM_GRAPH_H

#include <draw.h>

enum graph_modes {
  GRAPH_MODE_AUTO,
  GRAPH_MODE_PAL,
  GRAPH_MODE_NTSC,
  GRAPH_MODE_HDTV_480P,
  GRAPH_MODE_HDTV_576P,
  GRAPH_MODE_HDTV_720P,
};

#define GRAPH_MODE_FIELD 0
#define GRAPH_ENABLE 1
#define GRAPH_DISABLE 0

enum ztest {
  ZTEST_METHOD_ALLPASS,
};

void graph_wait_vsync();
void graph_enable_output();
void graph_disable_output();
int graph_set_mode(int interlace, int mode, int ffmd, int flicker_filter);
int graph_set_screen(int xo, int yo, int width, int height);
int graph_set_bgcolor(int r, int g, int b);
void graph_set_framebuffer_filtered(int addr, int width, enum draw_psm psm, int x, int y);
#endif
