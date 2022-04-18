
#include "../log.h"
#include <draw.h>

void graph_wait_vsync() {}

void graph_enable_output() {}
void graph_disable_output() {}

int graph_set_mode(int interlace, int mode, int ffmd, int flicker_filter) {
  trace("graph set mode: interlace=%d, mode=%d, ffmd=%d, flicker=%d", interlace,
        mode, ffmd, flicker_filter);
  return 0;
}

int graph_set_screen(int xo, int yo, int width, int height) {
  trace("graph set screen: %d, %d, %d, %d", xo, yo, width, height);
  return 0;
}

int graph_set_bgcolor(int r, int g, int b) {
  trace("graph bgcol = (%x, %x, %x)", r, g, b);
  return 0;
}

void graph_set_framebuffer_filtered(int addr, int width, enum draw_psm psm,
                                    int x, int y) {
  trace("set FB = %X (width=%d, psm=%d, x=%d, y=%d)", addr, width, psm, x, y);
}

void init_scr() {}
