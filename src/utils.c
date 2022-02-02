#include <stdio.h>

#include "utils.h"
#include "log.h"

/**
 * print a QWORD buffer as trace level log
 */
int print_buffer(qword_t *b, int len) {
#ifdef LOG_TRACE
  if (log_output_level >= LOG_LEVEL_TRACE) {
    info("-- buffer %p\n", b);
    for (int i = 0; i < len; i++) {
      printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
      b++;
    }
    info("-- /buffer\n");
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
