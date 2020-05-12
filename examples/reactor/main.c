#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "reactor.h"
#include "interp.h"
#include "task.h"
#include "model.h"

struct event_handler_fsm_t {
	event_handler_t eh;
	fsm_t* fsm;
};
typedef struct event_handler_fsm_t event_handler_fsm_t;

static void eh_fsm_func (struct event_handler_t* this)
{
  event_handler_fsm_t* eh = (event_handler_fsm_t*) this;
  static const struct timeval period = { 0, 200 * 1000 };
  fsm_fire (eh->fsm);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static event_handler_t* eh_new_fsm (fsm_t* fsm, int prio)
{
  event_handler_fsm_t* eh = (event_handler_fsm_t*) malloc(sizeof(event_handler_fsm_t));
  event_handler_init ((event_handler_t*) eh, prio, eh_fsm_func);
  eh->fsm = fsm;
  return (event_handler_t*) eh;
}

static char op;
static int setpoint;
static int setpoint_get (void) { return setpoint; }
static void setpoint_set (int val) { setpoint = val; }

static void* reactor_main (void* arg)
{
  reactor_init ();

  reactor_add_handler(eh_new_fsm(
      fsm_new_heater(setpoint_get, setpoint_set), 1));
  reactor_add_handler(eh_new_fsm(
      fsm_new_control(setpoint_get, setpoint_set, &op), 2));

  while (1) {
    reactor_handle_events ();
  }
}


int cmd_up (char* arg) { op = 'u'; return 0; }
int cmd_down (char* arg) { op = 'd'; return 0; }
int cmd_temp (char* arg) {
  extern int temp;
  if (*arg)
    temp = atoi(arg);
  else
    printf ("%d\n", temp);
  return 0;
}

int
main ()
{
  interp_addcmd ("up", cmd_up, "increment setpoint");
  interp_addcmd ("down", cmd_down, "decrement setpoint");
  interp_addcmd ("temp", cmd_temp, "get/set temperature");
  task_new ("reactor", reactor_main, 1000, 1000, 1, 1024);
  interp_run();
  return 0;
}

