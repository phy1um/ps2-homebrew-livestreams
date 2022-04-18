
#ifndef P2SIM_DMA_H
#define P2SIM_DMA_H

enum dma_channels {
  DMA_CHANNEL_GIF,
  DMA_CHANNEL_VIF0,
  DMA_CHANNEL_VIF1,
};

int dma_channel_initialize(int chan, void *handler, int flags);
void dma_channel_fast_waits(int chan);
int dma_channel_send_normal(int chan, void *data, int qwc, int flags, int spr);
int dma_channel_send_chain(int chan, void *data, int data_size, int flags, int spr);
void dma_wait_fast();

#endif
