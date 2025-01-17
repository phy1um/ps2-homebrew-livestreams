#define _EE
#include <p2g/core.h>
#include <p2g/gs.h>
#include <p2g/log.h>
#include <p2g/ps2draw.h>
#include <p2g/utils.h>

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

static void vu1_read_registers(qword_t regs[32]) {
  register void * out asm("s1");
  out = regs;
  __asm__ __volatile__(
    "li $s0, 1024 \n"
    "sync \n"
    "ctc2 $s0, $vi1 \n"
    "li $a1, 32 \n"
    "loop: \n"
    "vlqi $vf1, ($vi1++) \n"
    "sqc2 $vf1, 0(%[out]) \n"
    "addiu %[out], %[out], 16 \n"
    "addi $a1, $a1, -1 \n"
    "bgez $a1, loop \n"
    "sync.p \n"
    : : [out]"r"(out) : "s0", "a0", "memory"
  );
}

static void vu1_debug_dump(size_t memory_start, size_t memory_end) {
  info("dumping vu1 data for debugging\n");
  printf("# vu1 registers:\n");
  qword_t registers[32];
  registers[0].dw[0] = 0x98765432987654;
  vu1_read_registers(registers);
  for (int i = 0; i < 8; i++) {
    printf("VF%02d: %016llX %016llX\n", i, registers[i].dw[1], registers[i].dw[0]);
    printf("@ xyzw: %f %f %f %f\n", i, *((float*)&registers[i].sw[0]), *((float*)&registers[i].sw[1]), 
      *((float*)&registers[i].sw[2]), *((float*)&registers[i].sw[3]));
  }
  printf("# vu1 umem:\n", memory_start, memory_end);
  for (size_t i = memory_start; i < memory_end; i += 16) {
    char *addr = (char *) (0x1100C000 + i);
    uint32_t *addrw = (uint32_t *) addr;
    printf("$%08X: %08lX %08lX %08lX %08lX\n", memory_start + i, addrw[3], addrw[2], addrw[1], addrw[0]);
  }
}

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

void enable_vu1_debug() {
  __asm__ __volatile__(
      "li $a0, 0xC00 \n"
      "sync \n"
      "ctc2 $a0, $28 \n"
      "sync.p \n"
      );
}

void vu1_resume() {
  VIF1_FBRST = 8;
}

uint32_t read_vpu_stat() {
  register uint32_t v1 asm("v1");
  __asm__ __volatile__(
    "sync.p \n" 
    "cfc2 %[v1], $29 \n"
    : [v1] "=r"(v1) : :
  );
  return v1;
}


int main(int argc, char *argv[]) {
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

  enable_vu1_debug();

  while (1) {
    trace("START FRAME: %d", frame_count);
    draw_frame_start();

    // read stat
    uint32_t vpu_stat = read_vpu_stat();
    trace("VPU stat = %X", vpu_stat);
    if (vpu_stat & 0x400) {
      trace("VPU1 stop @ T bit");
      vu1_debug_dump(0, 0x100);
      vu1_resume();
    }


    if (first_frame) {
      draw_vu_upload_program(vu1prog_buffer, prog_file_size, 0x0, TARGET_VU1);      
      first_frame = 0;
    }
    draw_vu_begin_unpack_verts(VIF_CODE_UNPACK_V432, 0x151515, 6, 0);
    draw3d_mesh_triangles_cnt(buffer, vertex_count, VERT_SIZE);
    draw_vu_call_program(0x0);
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
