#include "reactor.h"
#include "timeval_utils.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct reactor_t {
  event_handler_t* ehs[10];
  int n_ehs;
} Reactor;

static Reactor r;

void 
event_handler_init (event_handler_t* eh, int prio, int fd,
                    eh_func_t handle_timeout,
                    eh_func_t handle_read,
                    eh_func_t handle_write,
                    eh_func_t handle_exception)
{
  eh->prio = prio;
  eh->fd = fd;
  gettimeofday (&eh->next_activation, NULL);
  eh->handle_timeout = handle_timeout;
  eh->handle_read = handle_read;
  eh->handle_write = handle_write;
  eh->handle_exception = handle_exception;
}

void event_handler_handle_timeout (event_handler_t* eh) { eh->handle_timeout (eh); }
void event_handler_handle_read (event_handler_t* eh) { eh->handle_read (eh); }
void event_handler_handle_write (event_handler_t* eh) { eh->handle_write (eh); }
void event_handler_handle_exception (event_handler_t* eh) { eh->handle_exception (eh); }

void reactor_init (void) { r.n_ehs = 0; }

int
compare_prio (const void* a, const void* b)
{
  event_handler_t* eh1 = *(event_handler_t**) a;
  event_handler_t* eh2 = *(event_handler_t**) b;
  if (eh1->prio > eh2->prio)
    return -1;
  if (eh1->prio < eh2->prio)
    return 1;
  return 0;
}

void
reactor_add_handler (event_handler_t* eh)
{
  r.ehs[r.n_ehs++] = eh;
  qsort (r.ehs, r.n_ehs, sizeof (event_handler_t*), compare_prio);
}

static struct timeval*
reactor_next_timeout (void)
{
  static struct timeval next;
  struct timeval now;
  int i;

  if (! r.n_ehs) return NULL;

  gettimeofday(&now, NULL);
  next.tv_sec = now.tv_sec + 24*60*60;
  next.tv_usec = now.tv_usec;

  for (i = 0; i < r.n_ehs; ++i) {
    event_handler_t* eh = r.ehs[i];
    if (timeval_less (&eh->next_activation, &next)) {
      next.tv_sec = eh->next_activation.tv_sec;
      next.tv_usec = eh->next_activation.tv_usec;
    }
  }
  if (timeval_less (&next, &now)) {
    next.tv_sec = now.tv_sec;
    next.tv_usec = now.tv_usec;
  }
  return &next;
}

void
reactor_handle_events (void)
{
  int i, ret, maxfd = -1;
  struct timeval timeout, now;
  struct timeval zero = {0, 0};
  struct timeval* next_activation = reactor_next_timeout();
  fd_set rdset, wrset, exset;

  FD_ZERO(&rdset); FD_ZERO(&wrset); FD_ZERO(&exset); 
  for (i = 0; i < r.n_ehs; ++i) {
    event_handler_t* eh = r.ehs[i];
    if (eh->handle_read)
	FD_SET(eh->fd, &rdset);
    if (eh->handle_write)
	FD_SET(eh->fd, &wrset);
    if (eh->handle_exception)
	FD_SET(eh->fd, &exset);
    if (eh->fd > maxfd)
	maxfd = eh->fd;
  }
  
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  if (timeval_less(&timeout, &zero))
    timeout.tv_sec = timeout.tv_usec = 0;
  ret = select (maxfd + 1, &rdset, &wrset, &exset, &timeout);
  if (ret < 0) {
    perror ("select");
    printf("maxfd = %d\n", maxfd);
    printf("timeout = %ld, %d\n", timeout.tv_sec, timeout.tv_usec);
    return;
  }

  gettimeofday (&now, NULL);
  for (i = 0; i < r.n_ehs; ++i) {
    event_handler_t* eh = r.ehs[i];
    if (FD_ISSET(eh->fd, &rdset)) {
      event_handler_handle_read (eh);
      break;
    }
    else if (FD_ISSET(eh->fd, &wrset)) {
      event_handler_handle_write (eh);
      break;
    }
    else if (FD_ISSET(eh->fd, &exset)) {
      event_handler_handle_exception (eh);
      break;
    }
    else if (eh->handle_timeout && timeval_less (&eh->next_activation, &now)) {
      event_handler_handle_timeout (eh);
      break;
    }
  }
}

