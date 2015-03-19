#include <stdio.h>
#include "reactor.h"

static
void
task1_func (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  static int cnt = 0;

  printf ("%d\n", cnt++);
  
  timeval_add (&this->next_activation, &this->next_activation, &period);
}


int
main ()
{
  EventHandler eh1;
  reactor_init ();

  event_handler_init (&eh1, 1, task1_func);
  reactor_add_handler (&eh1);

  while (1) {
    reactor_handle_events ();
  }
}
