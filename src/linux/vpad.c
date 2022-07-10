
#include <stdint.h>

void padInit(int i) {}

void padPortOpen(int port, int slot, void *p) {}

int32_t padGetState(int port, int slot) { return 0; }

int padInfoMode(int port, int slot, int mode, int i) { return 0; }

void padSetMainMode(int port, int slot, int a, int b) {}

int padGetReqState(int port, int slot) { return 0; }

int padRead(int port, int slot, void *p) { return 0; }
