
#ifndef P2SIM_KERNEL_H
#define P2SIM_KERNEL_H

#include <stdio.h>
#define scr_printf(m, ...) printf(m, ##__VA_ARGS__)

void FlushCache(int i);
int SifLoadModule(const char *mod, int i, int j);

#endif
