
#include <ps2ips.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>

#include "log.h"
#include "net.h"

#define THREAD_STACK_SIZE 4000

typedef int(*net_handler_fn)(int fd, void *arg, void *msg, size_t msg_len);


static int handle_message(int fd, struct net_state *state);
static void net_listen_loop(struct net_state *state);

extern void * _gp;

int net_init() {
  trace("net init: load PS2IPS.IRX");
  SifInitRpc(0);
  SifLoadModule("host:ps2ips.irx", 0 ,0);

  trace("loaded PS2IPS IRX, init ps2ip stack");
  if (ps2ip_init() < 0) {
    logerr("failed to start PS2IP");
    return 1;
  }
  
  return 0;
}

int net_listen(net_handler_fn handler, int port, size_t recv_len, void *arg, struct net_state *s) {

  if (!s) {
    logerr("net state is NULL");
    return 1;
  }
  
  if (s->is_init) {
    info("init called on initialized net state, returning");
    return 0;
  }

  trace("starting net listener - port = %d", port);

  s->port = port; 
  s->max_read = recv_len;
  s->read_buffer = malloc(recv_len);
  s->arg = arg;
  s->handler = handler;

  void *thread_stack = malloc(THREAD_STACK_SIZE);

  ee_thread_t thread_data = {
    .func = net_listen_loop,
    .stack = thread_stack,
    .stack_size = THREAD_STACK_SIZE,
    .gp_reg = &_gp,
    .initial_priority = 0x59,
    .attr = 0,
    .option = 0,
  };

  trace("creating thread");
  s->listen_id = CreateThread(&thread_data);

  if (s->listen_id < 0) {
    logerr("failed to start net listener thread");
    return 1;
  }

  s->is_init = 1;

  StartThread(s->listen_id, s);

  return 0;
}

static void net_listen_loop(struct net_state *state) {
  trace("listen thread started, port=%d, ID=%d", state->port, state->listen_id); 

  int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd < 0) {
    int err = errno;
    logerr("no create socket rc=%d (listener id = %d)", socket_fd, state->listen_id);
    logerr("ERRNO = %d", err);
    return;
  }

  struct sockaddr_in listen_addr = {0};
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  listen_addr.sin_port = htons(state->port);

  int bind_status = bind(socket_fd, (struct sockaddr *) &listen_addr, sizeof(listen_addr));

  if (bind_status < 0) {
    logerr("failed to bind socket %d (listener id = %d)", state->port, state->listen_id); 
    return;
  }

  int listen_status = listen(socket_fd, 2);
  if (listen_status < 0) {
    logerr("failed to listen on socket %d (listener id = %d)", socket_fd, state->listen_id);
    return;
  }

  trace("listen socket startup, running loop (id = %d)", state->listen_id);

  fd_set active_fd;
  FD_ZERO(&active_fd);
  FD_SET(socket_fd, &active_fd);
  while(1) {

    struct sockaddr_in client_addr = {0};
    int client_addr_len = sizeof(client_addr);
    fd_set rd_set = active_fd;
    if (select(FD_SETSIZE, &rd_set, 0, 0, 0) < 0) {
      logerr("failed to SELECT (listener id = %d)", state->listen_id);
      SleepThread();
    }

    for (int i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &rd_set)) {
        if (i == socket_fd) {
          int cs = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
          if (cs < 0) {
            logerr("failed to accept (listener id = %d)", state->listen_id);
            SleepThread();
          }
          FD_SET(cs, &active_fd);
        } else {
          if (handle_message(i, state) < 0) {
            info("disconnect client %d (listener id = %d)", i, state->listen_id);
            FD_CLR(i, &active_fd);
            disconnect(i);
          }
        }
      }
    }
  }

}

static int handle_message(int fd, struct net_state *state) {
  ssize_t recv_len = recv(fd, state->read_buffer, state->max_read, 0);
  if (recv_len <= 0) {
    logerr("failed to recv (listener_id = %d)", state->listen_id);
    return 1;
  }

  return state->handler(fd, state->arg, state->read_buffer, recv_len);
}
