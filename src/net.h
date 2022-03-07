
#ifndef PS2ENG_NET_H
#define PS2ENG_NET_H

typedef int(*net_handler_fn)(int fd, void *arg, void *msg, size_t msg_len);

struct net_state {
  int is_init;
  int port;
  int listen_id;
  size_t max_read;
  void *read_buffer;
  void *arg;
  net_handler_fn handler;
};


int net_init();
int net_listen(net_handler_fn handler, int port, size_t recv_len, void *arg, struct net_state *s);

#endif
