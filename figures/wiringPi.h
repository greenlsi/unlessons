#ifndef WIRINGPI_H
#define WIRINGPI_H

int wiringPiSetup (void);
int wiringPiISR (int pin, int edgeType, void (*isr)(void));

#define INT_EDGE_SETUP   0
#define INT_EDGE_FALLING 1
#define INT_EDGE_RISING  2
#define INT_EDGE_BOTH    3

/* interrupt simulation */
void wiringPi_gen_interrupt (int pin);

#endif
