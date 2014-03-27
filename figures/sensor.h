#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>

void sensor1_init();

void sensor1_task();

void sensorISR();
#endif
