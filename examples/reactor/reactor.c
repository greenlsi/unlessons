#include "reactor.h"
#include "timeval_utils.h"
#include <stdlib.h>

typedef struct reactor_t {
  event_handler_t* ehs[10];
  int n_ehs;
} Reactor;

static Reactor r;

void
event_handler_init (event_handler_t* eh, int prio, eh_func_t run)
{
  eh->prio = prio;
  gettimeofday (&eh->next_activation, NULL);
  eh->run = run;
}

void
event_handler_run (event_handler_t* eh)
{
  eh->run (eh);
}

void
reactor_init (void)
{
  r.n_ehs = 0;
}

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
  int i;
  struct timeval timeout, now;
  struct timeval* next_activation = reactor_next_timeout();

  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout);

  gettimeofday (&now, NULL);
  for (i = 0; i < r.n_ehs; ++i) {
    event_handler_t* eh = r.ehs[i];
    if (timeval_less (&eh->next_activation, &now)) {
      event_handler_run (eh);
    }
  }
}

