#include "reactor.h"
#include <stdlib.h>

int
timeval_less (const struct timeval* a, const struct timeval* b)
{
  return (a->tv_sec == b->tv_sec)? (a->tv_usec < b->tv_usec) :
    (a->tv_sec < b->tv_sec);
}

void
timeval_add (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;
  if (res->tv_usec >= 1000000) {
    res->tv_sec += res->tv_usec / 1000000;
    res->tv_usec = res->tv_usec % 1000000;
  }
}

void
timeval_sub (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    res->tv_sec --;
    res->tv_usec += 1000000;
  }
}


typedef struct reactor_t {
  EventHandler* ehs[10];
  int n_ehs;
} Reactor;

static Reactor r;

void
event_handler_init (EventHandler* eh, int prio, eh_func_t run)
{
  eh->prio = prio;
  gettimeofday (&eh->next_activation, NULL);
  eh->run = run;
}

void
event_handler_run (EventHandler* eh)
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
  EventHandler* eh1 = *(EventHandler**) a;
  EventHandler* eh2 = *(EventHandler**) b;
  if (eh1->prio > eh2->prio)
    return -1;
  if (eh1->prio < eh2->prio)
    return 1;
  return 0;
}

void
reactor_add_handler (EventHandler* eh)
{
  r.ehs[r.n_ehs++] = eh;
  qsort (r.ehs, r.n_ehs, sizeof (EventHandler*), compare_prio);
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
    EventHandler* eh = r.ehs[i];
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
    EventHandler* eh = r.ehs[i];
    if (timeval_less (&eh->next_activation, &now)) {
      event_handler_run (eh);
    }
  }
}

