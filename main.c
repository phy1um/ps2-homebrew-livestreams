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

#include "gs.h"
#include "mesh.h"
#include "log.h"

#define OFFSET_X 2048
#define OFFSET_Y 2048

#define myftoi4(x) (((uint64_t)(x))<<4)

#define VID_W 640
#define VID_H 448

#define TGT_FILE "host:cube.gif"

#define fatalerror(st, msg, ...) printf("FATAL: " msg "\n", ##__VA_ARGS__); error_forever(st); ((void)0)
void error_forever(struct draw_state *st);

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

void mesh_transform(struct model *m, char *b, int cx, int cy, float scale, float tx, float ty)
{
  int stride = m->vertex_size * 16;
  for (int i = 0; i < m->vertex_count; i++) {
    // get address of current vertex data
    float *pos = (float*) (b + (stride*i) + (m->vertex_position_offset*16));
    float x = pos[0];
    float y = pos[1];
    float z = pos[2];
    // float w = pos[3];
    *((uint32_t*)pos) = myftoi4((x * scale) + tx) + cx;
    *((uint32_t*)(pos+1)) = myftoi4((y * scale) + ty) + cy;
    *((uint32_t*)(pos+2)) = (int) z;
    pos[3] = 0;
  }
}

int main()
{
  printf("Hello\n");
  qword_t *buf = malloc(20000*16);
  char *file_load_buffer = malloc(310 * 1024);
  int file_load_buffer_len = 310*1024;

  struct draw_state st = {0};
  st.width = VID_W,
  st.height = VID_H,
  st.vmode = graph_get_region(),
  st.gmode = GRAPH_MODE_INTERLACED,

  // init DMAC
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);

  // initialize graphics mode 
  gs_init(&st, GS_PSM_32, GS_PSMZ_24);

  struct model m = {0};
  m.r = 0xff;
  int bytes_read = load_file(TGT_FILE, file_load_buffer, file_load_buffer_len);
  if (bytes_read <= 0) {
    fatalerror(&st, "failed to load file %s", TGT_FILE);
  }
  if (bytes_read % 16 != 0) {
    fatalerror(&st, "lengt of model file %s was not 0 %% 16", TGT_FILE);
  }

  if (!model_load(&m, file_load_buffer, bytes_read)) {
    fatalerror(&st, "failed to process model");
  }

  graph_wait_vsync();
  while(1) {
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, 20000*16);
    q = draw_disable_tests(q, 0, &st.zb);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, VID_W, VID_H, 20, 20, 20);
    q = draw_enable_tests(q, 0, &st.zb);
    qword_t *model_verts_start = q;
    memcpy(q, m.buffer, m.buffer_len);
    info("copied mesh buffer with len=%d", m.buffer_len);
    q += (m.buffer_len/16);
    q = draw_finish(q);
    mesh_transform(&m, (char *) (model_verts_start+4), myftoi4(2048), myftoi4(2048), 2.0f, 0, 0);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
    print_buffer(buf, q-buf); 
    info("draw from buffer with length %d", q-buf);

    draw_wait_finish();
    graph_wait_vsync();
    sleep(10);
  }
}


void error_forever(struct draw_state *st)
{
  qword_t *buf = malloc(1200);
  while(1) {
    dma_wait_fast();
    qword_t *q = buf;
    memset(buf, 0, 1200);
    q = draw_disable_tests(q, 0, &st->zb);
    q = draw_clear(q, 0, 2048.0f - 320, 2048.0f - 244, VID_W, VID_H, 0xff, 0, 0);
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q-buf, 0, 0);
    draw_wait_finish();
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
