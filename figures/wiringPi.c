#include <wiringPi.h>

static void (*isr[32]) (void);

int
wiringPiSetup (void)
{
  return 0;
}

int
wiringPiISR (int pin, int edgeType, void (*func)(void))
{
  isr[pin] = func;
  return 0;
}

void
wiringPi_gen_interrupt (int pin)
{
  if (isr[pin])
    (*isr[pin]) ();
}
