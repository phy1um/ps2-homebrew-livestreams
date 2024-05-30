#include <stdio.h>

#include <p2g/log.h>
#include <p2g/utils.h>

/**
 * print a QWORD buffer as trace level log
 */
int print_buffer(qword_t *b, int len) {
#ifdef LOG_TRACE
  if (log_output_level >= LOG_LEVEL_TRACE) {
    trace("-- buffer %p\n", b);
    for (int i = 0; i < len; i++) {
      printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
          b->b[0], b->b[1], b->b[2], b->b[3],
          b->b[4], b->b[5], b->b[6], b->b[7],
          b->b[8], b->b[9], b->b[10], b->b[11],
          b->b[12], b->b[13], b->b[14], b->b[15]
        );
      //printf("%08X %08X %08X %08X")
      //printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
      b++;
    }
    trace("-- /buffer\n");
  }
#endif
  return 0;
}

/**
 * get the last index of a character in a string
 */
int last_index_of(const char *str, int str_len, char c) {
  int ind = -1;
  for (int i = 0; i < str_len; i++) {
    if (str[i] == c) {
      ind = i;
    }
  }
  return ind;
}
