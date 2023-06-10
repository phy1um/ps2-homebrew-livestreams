
#ifndef P2G_DRAW_INTERNAL
#define P2G_DRAW_INTERNAL

int draw_clear_buffer();
int draw_update_last_tag_loops();
int draw_start_cnt();
int dmatag_raw(int qwc, int type, int addr);
int draw_end_cnt();
int draw_dma_end();
int draw_dma_ref(uint32_t addr);
int draw_kick();

#endif
