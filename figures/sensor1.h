#ifndef SENSOR1_H
#define SENSOR1_H

#include "observer.h"

#define NSENSOR1 4

typedef struct sensor1_t {
  observable_t observable;
  int id;
} sensor1_t;

extern sensor1_t* sensor1[NSENSOR1];

void sensor1_setup (int prio);

sensor1_t* sensor1_new (int id);
void sensor1_init (sensor1_t* this, int id);
void sensor1_process_data (sensor1_t* this);

#endif
