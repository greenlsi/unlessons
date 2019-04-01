#include "esp_common.h"
#include "reactor.h"

typedef struct reactor_t {
  EventHandler* ehs[10];
  int n_ehs;
} Reactor;

static Reactor r;

void
event_handler_init (EventHandler* eh, int prio, eh_func_t run)
{
  eh->prio = prio;
  eh->next_activation = xTaskGetTickCount();
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

static portTickType
reactor_next_timeout (void)
{
  static portTickType next;
  portTickType now = xTaskGetTickCount();
  int i;

  next = now + 60*60*1000 /portTICK_RATE_MS;
  if (! r.n_ehs) return next;

  for (i = 0; i < r.n_ehs; ++i) {
    EventHandler* eh = r.ehs[i];
    if (eh->next_activation < next)
      next = eh->next_activation;
  }
  if (next < now)
    next = now;
  return next;
}

void
reactor_handle_events (void)
{
  int i;
  portTickType timeout, now;
  portTickType next_activation = reactor_next_timeout();

  now = xTaskGetTickCount();
  timeout = next_activation - now;
  if (timeout > 0)
    vTaskDelay (timeout);

  now = xTaskGetTickCount();
  for (i = 0; i < r.n_ehs; ++i) {
    EventHandler* eh = r.ehs[i];
    if (eh->next_activation < now) {
      event_handler_run (eh);
    }
  }
}

