
#ifndef P2SIM_TAMTYPES_H
#define P2SIM_TAMTYPES_H

#include <stdint.h>

typedef union {
  uint8_t b[16];
  uint16_t hw[8];
  uint32_t sw[4];
  uint64_t dw[2];
  unsigned __int128 qw;
} qword_t;

#endif
