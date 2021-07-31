
#include <stdio.h>
#include <unistd.h>

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <dma.h>
#include <dma_tags.h>

#include <inttypes.h>

#define OFFSET_X 0
#define OFFSET_Y 0

int print_buffer(qword_t *b, int len)
{
  printf("-- buffer\n");
  for(int i = 0; i < len; i++) {
    printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
    b++;
  }
  printf("-- /buffer\n");
  return 0;
}

int gs_finish()
{
  qword_t buf[50];
  qword_t *q = buf;
  q = draw_primitive_xyoffset(q, 0, 0, 0);
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
  dma_wait_fast();
  return 0;
}

int gs_init(int width, int height, int psm, int psmz, int vmode, int gmode)
{
  unsigned int fb_address = graph_vram_allocate(width, height, psm, GRAPH_ALIGN_PAGE);
  //unsigned int zbuf_address = graph_vram_allocate(w, h, psmz, GRAPH_ALIGN_PAGE);
  graph_set_mode(gmode, vmode, GRAPH_MODE_FIELD, GRAPH_DISABLE); 
  graph_set_screen(OFFSET_X, OFFSET_Y, width, height);
  graph_set_bgcolor(0, 0, 0);
  graph_set_framebuffer_filtered(fb_address, width, psm, 0, 0);
  graph_enable_output();

  gs_finish();

  return 0;
}

static int tri[] = {
    10, 0, 0,
    600, 200, 0,
    20, 400, 0,
};

int draw()
{
  
  uint64_t red = 0xf0;
  uint64_t blue = 0x0f;
  uint64_t green = 0x0f;

  qword_t buf[50];
  qword_t *q = buf;
  // 6 regs, x1, EOP
  q->dw[1] = 0x6000000000008001;
  // GIFTag header - col, pos, col, pos, col, pos
  q->dw[0] = 0x0000000000515151;
  q++;

  for(int i = 0; i < 3; i++) {

    q->dw[0] = (red&0xff) | (green&0xff)<<32;
    q->dw[1] = (blue&0xff) | (0x80 << 32);
    q++;

    q->dw[0] = (tri[i+0]<<4) | (tri[i+1]<<4)<<32;
    q->dw[1] = (tri[i+2]<<4);
    q++;
  }

  print_buffer(buf, q-buf);
  dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
  dma_wait_fast();
  
  gs_finish();
  return 0;
}

int main()
{
  printf("Hello\n");
  // init DMAC
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);
  // initialize graphics mode 
  gs_init(664, 480, GS_PSM_32, GS_PSMZ_32, GRAPH_MODE_NTSC, GRAPH_MODE_FIELD);
  
  // clear
  draw();

  // build buffer with triangle data

  while(1)
	continue;
}
