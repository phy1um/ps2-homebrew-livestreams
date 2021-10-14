#include <lua.h>

#include <dma.h>
#include <dma_tags.h>
#include <tamtypes.h>

#include "log.h"

static int dma_init(lua_State *l) {
  int channel = lua_tointeger(l, 1); 
  info("doing dma init, channel = %d", channel);
  dma_channel_initialize(channel, 0, 0);
  dma_channel_fast_waits(channel);
  return 0;
}

int print_buffer(qword_t *b, int len) {
  info("-- buffer\n");
  for (int i = 0; i < len; i++) {
    printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
    b++;
  }
  info("-- /buffer\n");
  return 0;
}


static int dma_send_buffer(lua_State *l) {
  // buffer is arg 1
  lua_pushstring(l, "ptr");
  lua_gettable(l, 1);
  void *ptr = lua_touserdata(l, -1);
  lua_pushstring(l, "head");
  lua_gettable(l, 1);
  int head = lua_tointeger(l, -1);
  
  // channel is arg 2
  int channel = lua_tointeger(l, 2);

  // print buffer for debugging
  //t,print_buffer(ptr, head/16);

  // info("DMA send :: sending %d qwords on channel %d", head/16, channel);
  dma_channel_send_normal(channel, ptr, head/16, 0, 0);
  return 0;
}

static int dma_wait(lua_State *l) {
  dma_wait_fast();

  return 0;
}

int dma_lua_init(lua_State *l) {
  lua_createtable(l, 0, 5); 
  lua_pushcfunction(l, dma_wait);
  lua_setfield(l, -2, "waitFast");
  lua_pushcfunction(l, dma_send_buffer);
  lua_setfield(l, -2, "send");
  lua_pushcfunction(l, dma_init);
  lua_setfield(l, -2, "init");

  lua_pushinteger(l, DMA_CHANNEL_GIF);
  lua_setfield(l, -2, "GIF");
  lua_pushinteger(l, DMA_CHANNEL_VIF0);
  lua_setfield(l, -2, "VIF0");  
  lua_pushinteger(l, DMA_CHANNEL_VIF1);
  lua_setfield(l, -2, "VIF1");

  lua_setglobal(l, "DMA");
  return 0;
}
