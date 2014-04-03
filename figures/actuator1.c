#include <stdlib.h>
#include "sensor1.h"
#include "actuator1.h"
#include "screen.h"

void
actuator1_setup (void)
{
  actuator1_t* actuator = actuator1_new (1);
  observable_register_observer ((observable_t*) sensor1[0],
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
actuator1_notify (observer_t* observer)
{
  static int x;
  screen_printxy (x++, 0, ".");
}

