#include <stdio.h>
#include <unistd.h>
#include "reactor.h"

static
void
task1_func (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  static int cnt = 0;

  printf ("1: (%d)\n", cnt++);
  
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static void task2_part1(void) { printf ("2:1\n"); sleep (1); }
static void task2_part2(void) { printf ("2:2\n"); sleep (1); }
static void task2_part3(void) { printf ("2:3\n"); sleep (1); }
static void task2_part4(void) { printf ("2:4\n"); sleep (1); }

static
void
task2_func (struct event_handler_t* this)
{
  static const struct timeval period = { 3, 0 };
  static int frame = 0;

  switch (frame) {
  case 0: 
    task2_part1(); 
    break;
  case 1: 
    task2_part2(); 
    break;
  case 2: 
    task2_part3(); 
    break;
  case 3: 
    task2_part4();
    timeval_add (&this->next_activation, &this->next_activation, &period);
    break;
  }
  frame = (frame + 1) % 4;
}


int
main ()
{
  EventHandler eh1, eh2;
  reactor_init ();

  event_handler_init (&eh1, 2, task1_func);
  reactor_add_handler (&eh1);

  event_handler_init (&eh2, 1, task2_func);
  reactor_add_handler (&eh2);

  while (1) {
    reactor_handle_events ();
  }
}
