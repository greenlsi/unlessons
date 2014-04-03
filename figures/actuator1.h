#ifndef ACTUATOR1_H
#define ACTUATOR1_H

#include "observer.h"

typedef struct actuator1_t {
  observable_t observer;
  int id;
} actuator1_t;

void actuator1_setup (void);

actuator1_t* actuator1_new (int id);
void actuator1_init (actuator1_t* this, int id);
void actuator1_notify (observer_t* this);

#endif
