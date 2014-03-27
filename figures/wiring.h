#ifndef WIRING_H
#define WIRING_H

#include <pthread.h>

int wiringPiISR (int pin, int edgeType,  void (*function)(void)) ;

#endif
