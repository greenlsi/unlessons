#ifndef WIRINGPI_H
#define WIRINGPI_H

#define LOW 0
#define HIGH 1

#define wiringPiSetup()
#define pinMode(gpio,mode)
#define wiringPiISR(gpio,mode,func)

void digitalWrite(int gpio, int value);

#endif
