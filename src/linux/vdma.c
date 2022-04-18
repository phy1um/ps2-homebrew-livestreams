#define DMA_CNT 0x1
#define DMA_REF 0x3
#define DMA_END 0x7

#include "../log.h"

void channel_process(int chan_id, void *data, int len) {
  info("channel: sink @ %p, size=%d", data, len);
  return;
}

int dma_channel_initialize(int chan, void *handler, int flags) {
  info("DMA chan init: %d, handler=%p, flags=%x", chan, handler, flags);
  return 0;
}

void dma_channel_fast_waits(int chan) {
  info("DMA chan %d: fast waits enabled", chan);
}

int dma_channel_send_normal(int chan, void *data, int qwc, int flags, int spr) {
  channel_process(chan, data, qwc*16);
}

int dma_channel_send_chain(int chan, void *data, int data_size, int flags, int spr) {
  int hb = 0;
  while(hb < data_size) {
    uint32_t t0 = *((uint32_t*) (data + hb));
    uint32_t addr = *((uint32_t*) (data + hb + 4));
    unsigned int qwc = t0 & 0xffff;
    unsigned int type = (t0 >> 28) & 0xf;
    switch(type) {
      case DMA_CNT:
        channel_process(chan, data+hb+8, qwc*16);
        hb += qwc*16 + 8;
        break;
      case DMA_REF:
        channel_process(chan, addr, qwc*16);
        hb += 8;
        break;
      case DMA_END:
        return 0;
      default:
        logerr("unsupported DMA type: %d", type);
        hb += 8;
        break;
    }
  }
}

void dma_wait_fast() {
  info("DMA wait (fast)...");
}
