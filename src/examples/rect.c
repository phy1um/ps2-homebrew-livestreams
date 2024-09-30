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

// normal deps
#include <stdlib.h>

#define SCR_WIDTH 640
#define SCR_HEIGHT 448

static uint32_t VRAM_HEAD = 0;
static uint32_t VRAM_SIZE = 4*1024*1024;
uint32_t vram_alloc(uint32_t size, uint32_t align) {
  trace("vram alloc: %d", size);
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
  log_output_level = LOG_LEVEL_TRACE;

  gs_init();
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  gs_set_output(
      SCR_WIDTH, SCR_HEIGHT,
      GRAPH_MODE_INTERLACED,
      GRAPH_MODE_NTSC,
      GRAPH_MODE_FIELD,
      GRAPH_DISABLE
   );

 
  int fb1 = vram_alloc(
      gs_framebuffer_size(SCR_WIDTH,SCR_HEIGHT,GS_PSM_32),
      2048 
    );
  int fb2 = vram_alloc(
      gs_framebuffer_size(SCR_WIDTH,SCR_HEIGHT,GS_PSM_32),
      2048
    );

  int zbuf = vram_alloc(
      gs_framebuffer_size(SCR_WIDTH,SCR_HEIGHT,GS_PSMZ_16),
      2048
    );
  trace("allocated buffers: FB1=%x, FB2=%x, ZB=%x", fb1, fb2, zbuf);
  gs_set_fields(SCR_WIDTH, SCR_HEIGHT, GS_PSM_32, GS_PSMZ_16, fb1/4, fb2/4, zbuf/4);

  void *draw_buffer_static = calloc(1, 200*1024);
  // setup drawbuffer
  draw_bind_buffer(draw_buffer_static, 200*1024);

  draw2d_screen_dimensions(SCR_WIDTH, SCR_HEIGHT);

  int frame_count = 0;
        
  while(1) {
    trace("START FRAME: %d", frame_count);
    dma_wait_fast();
    trace("FRAME START");
    draw_frame_start();
    draw2d_set_colour(255, 0, 0, 0x80);
    draw2d_rect(100., 80., 100., 80.);
    trace("FRAME END");
    draw_frame_end();
    trace("WAIT FINISH");
    draw_wait_finish();
    trace("WAIT VSYNC: (%d)", frame_count);
    graph_wait_vsync();
    trace("FLIP");
    gs_flip();
    frame_count += 1;
  }
}
