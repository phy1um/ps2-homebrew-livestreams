
#include <p2g/core.h>
#include <p2g/log.h>

void FlushCache(int i) { return; }

int SifLoadModule(const char *mod, int i, int j) {
  trace("SifLoadModule: %s", mod);
  return P2G_OK;
}
