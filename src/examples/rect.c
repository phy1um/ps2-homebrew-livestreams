#define _EE
#include <p2g/log.h>
#include <p2g/gs.h>
#include <p2g/utils.h>
#include <p2g/ps2draw.h>
#include <p2g/core.h>

// PS2SDK deps
#include <gs_psm.h>
#include <dma.h>
#include <graph.h>

static uint32_t VRAM_HEAD = 0;
static uint32_t VRAM_SIZE = 4*1024*1024;
uint32_t vram_alloc(uint32_t size, uint32_t align) {
  while (VRAM_HEAD%align != 0) {
    VRAM_HEAD += 1;
  }
  uint32_t out = VRAM_HEAD;
  if (out > VRAM_SIZE) {
    p2g_fatal("VRAM overflow");
  }
  VRAM_HEAD += size;
  return out;
}

int main(int argc, char *argv[]) {
  gs_init();
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  gs_set_output(
      640, 480,
      GRAPH_MODE_INTERLACED,
      GRAPH_MODE_NTSC,
      GRAPH_MODE_FIELD,
      GRAPH_DISABLE
    );

  int fb1 = vram_alloc(
      gs_framebuffer_size(640,448,GS_PSM_32),
      2048 
    )/4;
  int fb2 = vram_alloc(
      gs_framebuffer_size(640,448,GS_PSM_32),
      2048
    )/4;

  int zbuf = vram_alloc(
      gs_framebuffer_size(640,448,GS_PSMZ_16),
      2048
    )/4;
        
  while(1) {
    dma_wait_fast();
    draw_frame_start();
    draw2d_set_colour(255, 0, 0, 0x80);
    draw2d_rect(100.0, 80.0, 42.2, 100.0);
    draw_frame_end();
  }
}
