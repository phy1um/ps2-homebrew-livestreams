#define _EE
#include <p2g/core.h>
#include <p2g/gs.h>
#include <p2g/log.h>
#include <p2g/ps2draw.h>
#include <p2g/utils.h>
#include <p2g/pad.h>

// PS2SDK deps
#include <dma.h>
#include <graph.h>
#include <gs_psm.h>
#include <kernel.h>
#include <vif_registers.h>

// normal deps
#include <stdlib.h>

#include "../draw/vu.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 448
// vertex = 4x f32 XYZW, 4x u32 RGBA
#define VERT_SIZE ((4*4) + (4*4))

static uint32_t VRAM_HEAD = 0;
static uint32_t VRAM_SIZE = 4 * 1024 * 1024;
uint32_t vram_alloc(uint32_t size, uint32_t align) {
  trace("vram alloc: %d", size);
  while (VRAM_HEAD % align != 0) {
    VRAM_HEAD += 1;
  }
  uint32_t out = VRAM_HEAD;
  if (out > VRAM_SIZE) {
    p2g_fatal("VRAM overflow");
  }
  VRAM_HEAD += size;
  return out;
}

int appmain(int argc, char *argv[]) {
  log_output_level = LOG_LEVEL_TRACE;

  gs_init();
  dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_GIF);
  dma_channel_initialize(DMA_CHANNEL_VIF1, 0, 0);
  dma_channel_fast_waits(DMA_CHANNEL_VIF1);

  gs_set_output(SCR_WIDTH, SCR_HEIGHT, GRAPH_MODE_INTERLACED, GRAPH_MODE_NTSC,
                GRAPH_MODE_FIELD, GRAPH_DISABLE);

  int fb1 =
      vram_alloc(gs_framebuffer_size(SCR_WIDTH, SCR_HEIGHT, GS_PSM_32), 2048);
  int fb2 =
      vram_alloc(gs_framebuffer_size(SCR_WIDTH, SCR_HEIGHT, GS_PSM_32), 2048);

  int zbuf =
      vram_alloc(gs_framebuffer_size(SCR_WIDTH, SCR_HEIGHT, GS_PSMZ_16), 2048);
  trace("allocated buffers: FB1=%x, FB2=%x, ZB=%x", fb1, fb2, zbuf);
  gs_set_fields(SCR_WIDTH, SCR_HEIGHT, GS_PSM_32, GS_PSMZ_16S, fb1 / 4, fb2 / 4,
                zbuf / 4);

  void *draw_buffer_static = calloc(1, 200 * 1024);
  // setup drawbuffer
  draw_bind_buffer(draw_buffer_static, 200 * 1024);
  draw_set_target(1);

  draw2d_screen_dimensions(SCR_WIDTH, SCR_HEIGHT);

  int frame_count = 0;

  FILE *model_file = fopen("cube.bin", "rb");
  if (!model_file) {
    p2g_fatal("no mesh file");
    return 1;
  }
  fseek(model_file, 0L, SEEK_END);
  long model_file_size = ftell(model_file);
  fseek(model_file, 0L, SEEK_SET);
  char *buffer = malloc(model_file_size);
  fread(buffer, 1, model_file_size, model_file);
  fclose(model_file);

  FILE *vuprog_file = fopen("ftoi.vuprog", "rb");
  if (!vuprog_file) {
    p2g_fatal("no vuprog file");
    return 1;
  }
  fseek(vuprog_file, 0L, SEEK_END);
  long prog_file_size = ftell(vuprog_file);
  fseek(vuprog_file, 0L, SEEK_SET);
  char *vu1prog_buffer = malloc(prog_file_size);
  fread(vu1prog_buffer, 1, prog_file_size, vuprog_file);
  fclose(vuprog_file);

  size_t vertex_count = model_file_size / VERT_SIZE;
  info("found verts: %zu (x %zu bytes)", vertex_count, VERT_SIZE);

  int first_frame = 1;

  pad_init();

  while (1) {
    trace("START FRAME: %d", frame_count);
    draw_frame_start();

    if (first_frame) {
      draw_vu_upload_program(vu1prog_buffer, prog_file_size, 0x0, TARGET_VU1);      
      first_frame = 0;
    }
    float mvp_matrix[16];
    mvp_matrix[0] = 40.f;
    mvp_matrix[1] = 40.f;
    mvp_matrix[2] = 40.f;
    draw_vu_begin_unpack_verts(VIF_CODE_UNPACK_V432, 0x151515, 6, 0, mvp_matrix, 16*sizeof(float));
    draw3d_mesh_triangles_cnt(buffer, vertex_count, VERT_SIZE);
    draw_vu_call_program(0x0);
    trace("FRAME END");
    draw_frame_end();

    trace("FLIP");
    gs_flip();
    frame_count += 1;
    SleepThread();

  }
}

int main(int argc, char *argv[]) {
  struct p2g_app app = {
    .main = appmain,
  };
  if(p2g_app_init(&app)) {
    p2g_fatal("startup error");
  }
  int rv = p2g_app_run(&app, argc, argv);
  p2g_fatal("app main returned");
}


