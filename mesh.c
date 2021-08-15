
#include "mesh.h"
#include "log.h"

#include <tamtypes.h>
#include <stdio.h>
#include <gs_gp.h>
#include <string.h>
#include <stdlib.h>

#define myftoi4(x) ((x)<<4)

int load_file(const char *fname, char *b, int b_len)
{
  FILE *f = fopen(fname, "rb");
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (len >= b_len) {
    return 0;
  }
  int byte_read = fread(b, 1, len, f);
  fclose(f);
  return byte_read;
}

#define MODEL_BUFFER_PRE (4*16)

// b_len MUST be a multiple of 16
int model_load(struct model *m, char *b, int b_len)
{
  if (!m || !b || b_len <= 0) {
    return 0;
  }
  m->buffer = malloc(b_len + MODEL_BUFFER_PRE);
  m->buffer_len = b_len + MODEL_BUFFER_PRE;
  qword_t *q = m->buffer; 
  m->vertex_size = 2;
  m->vertex_count = b_len / (16 * m->vertex_size);
  m->vertex_position_offset = 1;
  m->vertex_colour_offset = 0;
  m->face_count = m->vertex_count / 3;
  info("initializing model: verts=%d, faces=%d, bytes in buf=%d", m->vertex_count,
      m->face_count, m->buffer_len);

  // Create giftag, set regs via A+D
  q->dw[0] = 0x1000000000000001;
  q->dw[1] = 0x000000000000000e;
  q++;
  // set PRIM = triangle
  q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 1, 0, 0, 0, 0, 0, 0, 0);
  q->dw[1] = GS_REG_PRIM;
  q++;
  // start vertex data GIFTAG
  q->dw[0] = 0x6000000000000000 | (m->face_count & 0x3fff);
  q->dw[1] = 0x0000000000515151;
  q++;
  memcpy(q,b,b_len);
  return 1;
}


