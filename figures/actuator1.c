#include <stdlib.h>
#include "sensor1.h"
#include "actuator1.h"
#include "screen.h"
#include "fsm.h"

enum actuator1_state {
  ACT1_STATE1,
  ACT1_STATE2
};

static
void actuator1_enter_state2 (fsm_t* fsm)
{
  screen_printxy (0, 0, "*");
}

static
void actuator1_exit_state2 (fsm_t* fsm)
{
  screen_printxy (0, 0, ".");
}

static fsm_trans_t actuator1_fsm_tt[] = {
  { ACT1_STATE1, 0, ACT1_STATE2, actuator1_enter_state2 },
  { ACT1_STATE2, 1, ACT1_STATE1, actuator1_exit_state2 },
  { -1, -1, -1, NULL },
};

static fsm_t* actuator1_fsm;


void
actuator1_setup (void)
{
  actuator1_t* actuator = actuator1_new (1);
  actuator1_fsm = fsm_new (actuator1_fsm_tt);
  observable_register_observer ((observable_t*) sensor1[0],
                                (observer_t*) actuator);
  observable_register_observer ((observable_t*) sensor1[1],
                                (observer_t*) actuator);
}

actuator1_t*
actuator1_new (int id)
{
  actuator1_t* this = (actuator1_t*) malloc (sizeof (actuator1_t));
  actuator1_init (this, id);
  return this;
}

void
actuator1_init (actuator1_t* this, int id)
{
  observer_init ((observer_t*) this, actuator1_notify);
  this->id = id;
}

void
actuator1_notify (observer_t* observer, observable_t* observable)
{
  sensor1_t* sensor = (sensor1_t*) observable;
  fsm_fire (actuator1_fsm, sensor->id);
}

