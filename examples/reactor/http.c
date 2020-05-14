#include "reactor.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>

struct event_handler_http_t {
  event_handler_t eh;
};
typedef struct event_handler_http_t event_handler_http_t;

static void http_handle_read (struct event_handler_t* this)
{
  int cnt;
  char buf[1024];
  event_handler_http_t* eh = (event_handler_http_t*) this;
  cnt = read (eh->eh.fd, buf, 1024);
  write (1, buf, cnt);
}

event_handler_t* 
eh_new_http (int fd)
{
  event_handler_http_t* eh = (event_handler_http_t*) malloc(sizeof(event_handler_http_t));
  event_handler_init ((event_handler_t*) eh, 1, fd, NULL, http_handle_read, NULL, NULL);
  return (event_handler_t*) eh;
}


