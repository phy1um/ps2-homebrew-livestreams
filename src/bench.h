#ifndef BENCH_H
#define BENCH_H

#include <time.h>

#define BENCH_START(vname) clock_t vname = clock()
#define BENCH_INFO(vname, m)                                                   \
  do {                                                                         \
    clock_t __time_now = clock();                                                      \
    info(m, ((float)__time_now - vname) / (float)CLOCKS_PER_SEC);              \
  } while (0)


#endif
