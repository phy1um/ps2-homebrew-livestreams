
#include <lua.h>
#include <stdlib.h>
#include <ps2ips.h>

#include "net.h"
#include "log.h"

static int lua_run_msg(int fd, void *arg, void *msg, size_t msg_len) {
  //lua_State *l = arg;
  char *data = msg;
  data[msg_len - 1] = 0;
  info("got msg: %s", data);
  send(fd, msg, msg_len, 0);
  return 0;
}

static int listen_to_port(lua_State *l) {
  int port = lua_tointeger(l, 1);
  struct net_state *st = calloc(1, sizeof(struct net_state));
  int rc = net_listen(lua_run_msg, port, 4 * 1024, l, st);
  if (rc) {
    lua_pushstring(l, "failed to start listen server");
    lua_error(l);
    // lua_error does not return
  }
  lua_pushlightuserdata(l, st);
  return 1;
}

void listen_lua_init(lua_State *l) {
  lua_createtable(l, 0, 2);
  lua_pushcfunction(l, listen_to_port);
  lua_setfield(l, -2, "port");
  lua_setglobal(l, "LISTEN");
}
