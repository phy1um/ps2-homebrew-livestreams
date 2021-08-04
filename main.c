#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <dma.h>
#include <dma_tags.h>

#include <inttypes.h>

#define OFFSET_X 2048
#define OFFSET_Y 2048

#define myftoi4(x) ((x)<<4)

#define VID_W 640
#define VID_H 448

static qword_t *buf; 

#define DRAWBUF_LEN (100 * 16)

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
  qword_t *q = buf;
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
  dma_wait_fast();
  return 0;
}


zbuffer_t *z;
int gs_init(int width, int height, int psm, int psmz, int vmode, int gmode)
{
  framebuffer_t fb;
  fb.address = graph_vram_allocate(width, height, psm, GRAPH_ALIGN_PAGE);
  fb.width = width;
  fb.height = height;
  fb.psm = psm;
  fb.mask = 0;

  z->address = graph_vram_allocate(width, height, psmz, GRAPH_ALIGN_PAGE);
  z->enable = 0;
  z->method = 0;
  z->zsm = 0;
  z->mask = 0;

  graph_set_mode(gmode, vmode, GRAPH_MODE_FIELD, GRAPH_DISABLE); 
  graph_set_screen(0, 0, width, height);
  graph_set_bgcolor(0, 0, 0);
  graph_set_framebuffer_filtered(fb.address, width, psm, 0, 0);
  graph_enable_output();

  qword_t *q = buf;
  memset(buf, 0, DRAWBUF_LEN);
  q = draw_setup_environment(q, 0, &fb, z);
  q = draw_primitive_xyoffset(q, 0, 2048-(VID_W/2), 2048-(VID_H/2));
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
  draw_wait_finish();


  return 0;
}

static int tri[] = {
  10, 10, 0,
  500, 20, 0,
  300, 400, 0,
};

#define SHIFT_AS_I64(x, b) (((int64_t)x)<<b)

qword_t *draw(qword_t *q)
{
  
  uint64_t red = 0xf0;
  uint64_t blue = 0x0f;
  uint64_t green = 0x0f;

  // SET PRIM
  q->dw[0] = 0x1000000000000001;
  q->dw[1] = 0x000000000000000e;
  q++;
  q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 0, 0, 0, 0, 0, 0, 0, 0);
  q->dw[1] = GS_REG_PRIM;
  q++;
  // 6 regs, x1, EOP
  q->dw[0] = 0x6000000000008001;
  // GIFTag header - col, pos, col, pos, col, pos
  q->dw[1] = 0x0000000000515151;
  q++;

  int cx = myftoi4(2048 - (VID_W/2));
  int cy = myftoi4(2048 - (VID_H/2));

  for(int i = 0; i < 3; i++) {

    q->dw[0] = (red&0xff) | (green&0xff)<<32;
    q->dw[1] = (blue&0xff) | (SHIFT_AS_I64(0x80, 32));
    q++;
    
    // 0xa -> 0xa0
    // fixed point format - xxxx xxxx xxxx.yyyy
    int base = i*3;
    int x = myftoi4(tri[base+0]) + cx;
    int y = myftoi4(tri[base+1]) + cy;
    int z = 0;
    q->dw[0] = x | SHIFT_AS_I64(y, 32);
    q->dw[1] = z; 
    // printf("drawing vertex %x %x %x\n", tri[base+0], tri[base+1], tri[base+2]);
    q++;
  }

  return q;
}

int main()
{
  printf("Hello\n");
  buf = malloc(DRAWBUF_LEN);
  z = malloc(sizeof(zbuffer_t));
  // init DMAC
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  int vmode = graph_get_region();
  // initialize graphics mode 
  gs_init(VID_W, VID_H, GS_PSM_32, GS_PSMZ_24, vmode, GRAPH_MODE_INTERLACED);

  graph_wait_vsync();
  while(1) {
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, DRAWBUF_LEN);
    q = draw_disable_tests(q, 0, z);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, VID_W, VID_H, 20, 20, 20);
    q = draw_enable_tests(q, 0, z);
    q = draw(q);
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
    print_buffer(buf, q-buf); 

    draw_wait_finish();
    // wait for vsync
    graph_wait_vsync();
    sleep(2);
  }
}

/*
 *
 * xxxx xxxx xxxx . yyyy
 * 0100 0000 0000 . 0000
 *
 */
