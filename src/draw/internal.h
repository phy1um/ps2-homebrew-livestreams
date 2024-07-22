
#ifndef P2G_DRAW_INTERNAL
#define P2G_DRAW_INTERNAL

int draw_clear_buffer();
int draw_update_last_tag_loops();
int draw_start_cnt();
int dmatag_raw(struct commandbuffer *c, int qwc, int type, int addr);
int draw_end_cnt();
int draw_dma_end();
int draw_vifcode_direct_start(struct commandbuffer *c);
int draw_vifcode_end(struct commandbuffer *c);
// transfer qwc quad-words from addr, then read next tag
int draw_dma_ref(struct commandbuffer *c, uint32_t addr, int qwc);
int draw_kick();
int draw_kick_vif(struct commandbuffer *c);
int draw_kick_gif(struct commandbuffer *c);
int commandbuffer_update_last_tag_loop(struct commandbuffer *c);

#endif
