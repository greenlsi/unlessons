#ifndef REACTOR_H
#define REACTOR_H

#include "freertos/task.h"

struct event_handler_t;
typedef void (*eh_func_t) (struct event_handler_t*);

struct event_handler_t {
  int prio;
  portTickType next_activation;
  eh_func_t run;
};
typedef struct event_handler_t EventHandler;

void event_handler_init (EventHandler* eh, int prio, eh_func_t run);
void event_handler_run (EventHandler* eh);

void reactor_init (void);
void reactor_add_handler (EventHandler* eh);
void reactor_handle_events (void);

#endif
