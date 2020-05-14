#include <sys/select.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>
#include "reactor.h"

struct event_handler_acceptor_t {
  event_handler_t eh;
  int port;
  int qlen;
  struct sockaddr_in server;
  eh_new_fd_t eh_new;
};
typedef struct event_handler_acceptor_t event_handler_acceptor_t;

static void acceptor_handle_read (struct event_handler_t* this)
{
  int ss;
  event_handler_acceptor_t* eh = (event_handler_acceptor_t*) this;
  struct sockaddr_in client;
  socklen_t sin_size = sizeof(struct sockaddr_in);
  printf("acceptor --> accepting\n");
  ss = accept (eh->eh.fd, (struct sockaddr*) &client, &sin_size);
  printf("acceptor --> creating new handler(%d)\n", ss);
  reactor_add_handler (eh->eh_new(ss));
}

event_handler_t* 
eh_new_acceptor (int prio, int port, eh_new_fd_t eh_new)
{
  event_handler_acceptor_t* eh = (event_handler_acceptor_t*) malloc(sizeof(event_handler_acceptor_t));
  event_handler_init ((event_handler_t*) eh, prio, -1, NULL, acceptor_handle_read, NULL, NULL);
  eh->eh.fd = socket(PF_INET, SOCK_STREAM, 0);
  eh->eh_new = eh_new;

  bzero(&eh->server, sizeof(struct sockaddr_in));
  eh->server.sin_family = AF_INET;
  eh->server.sin_port = htons(eh->port = port);
  eh->server.sin_addr.s_addr = INADDR_ANY;

  bind(eh->eh.fd, (struct sockaddr*) &eh->server, sizeof(struct sockaddr));
  listen(eh->eh.fd, eh->qlen = 10);
  return (event_handler_t*) eh;
}

