
#ifndef GS_H
#define GS_H

int gs_init();
int gs_flip();
int gs_set_fields(int width, int height, int fmt, int zfmt, int fb1_addr,
    int fb2_addr, int zbuf_addr);
int gs_set_output(int width, int height, int interlace, int mode, int ffmd,
    int filter_flicker);

#endif
