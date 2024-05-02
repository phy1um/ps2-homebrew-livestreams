#define DMA_CNT 0x1
#define DMA_REF 0x3
#define DMA_END 0x7

#include <p2g/log.h>
#include <stdint.h>

char *CHANNEL_NAMES[] = {
  "VIF0", "VIF1", "GIF", "??", 
  "??", "??", "??", "??", 
  "??", "??", "!!OOB!!",
};

void channel_process(int chan_id, void *data, int len) {
  info("channel %s(%d): sink @ %p, size=%d", CHANNEL_NAMES[chan_id], chan_id, data, len);
  return;
}

int dma_channel_initialize(int chan, void *handler, int flags) {
  info("DMA chan init: %s(%d), handler=%p, flags=%x", CHANNEL_NAMES[chan], chan, handler, flags);
  return 0;
}

void dma_channel_fast_waits(int chan) {
  info("DMA chan %s(%d): fast waits enabled", CHANNEL_NAMES[chan], chan);
}

int dma_channel_send_normal(int chan, void *data, int qwc, int flags, int spr) {
  channel_process(chan, data, qwc * 16);
  return 0;
}

int dma_channel_send_chain(int chan, void *data, int data_size, int flags,
                           int spr) {
  trace("dma send chain: %p (%d) -> channel %s(%d)", data, data_size * 16, CHANNEL_NAMES[chan], chan);
  int hb = 0;
  while (hb < data_size * 16) {
    uint32_t t0 = *((uint32_t *)(data + hb));
    uint32_t addr = *((uint32_t *)(data + hb + 4));
    unsigned int qwc = t0 & 0xffff;
    unsigned int type = (t0 >> 28) & 0x7;
    trace("parse dma tag: %08X (addr = %04X qwc = %04X type = %02X", t0, addr, qwc, type);
    switch (type) {
    case DMA_CNT:
      channel_process(chan, data + hb + 8, qwc * 16);
      hb += (qwc+1) * 16;
      break;
    case DMA_REF:
      channel_process(chan, ((void *)&addr), qwc * 16);
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
  logerr("ERROR: DMA chain overrun");
  return 1;
}

void dma_wait_fast() { info("DMA wait (fast)..."); }
