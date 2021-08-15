
#include "gs.h"

#include <tamtypes.h>

#include <dma.h>
#include <dma_tags.h>

#include <draw.h>
#include <graph.h>

#include <string.h>
#include <stdlib.h>


#define GS_OFFSET_X 2048
#define GS_OFFSET_Y 2048

#define GS_CMD_BUFFER_LEN (40*16)
static qword_t *gs_cmd_buffer;

int gs_init(struct draw_state *ds, int psm, int psmz)
{
  if (!gs_cmd_buffer) {
    gs_cmd_buffer = malloc(GS_CMD_BUFFER_LEN);
  }

  ds->fb.address = graph_vram_allocate(ds->width, ds->height, psm, GRAPH_ALIGN_PAGE);
  ds->fb.width = ds->width;
  ds->fb.height = ds->height;
  ds->fb.psm = psm;
  ds->fb.mask = 0;

  ds->zb.address = graph_vram_allocate(ds->width, ds->height, psmz, GRAPH_ALIGN_PAGE);
  ds->zb.enable = 1;
  ds->zb.method = ZTEST_METHOD_GREATER;
  ds->zb.zsm = psmz;
  ds->zb.mask = 0;

  graph_set_mode(ds->gmode, ds->vmode, GRAPH_MODE_FIELD, GRAPH_DISABLE); 
  graph_set_screen(0, 0, ds->width, ds->height);
  graph_set_bgcolor(0, 0, 0);
  graph_set_framebuffer_filtered(ds->fb.address, ds->width, psm, 0, 0);
  graph_enable_output();

  qword_t *q = gs_cmd_buffer;
  memset(gs_cmd_buffer, 0, GS_CMD_BUFFER_LEN);
  q = draw_setup_environment(q, 0, &ds->fb, &ds->zb);
  q = draw_primitive_xyoffset(q, 0, GS_OFFSET_X-(ds->width/2), GS_OFFSET_Y-(ds->height/2));
  q = draw_finish(q);
  dma_channel_send_normal(DMA_CHANNEL_GIF, gs_cmd_buffer, q-gs_cmd_buffer, 0, 0);
  draw_wait_finish();

  return 0;
}


