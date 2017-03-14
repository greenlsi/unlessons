#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

void 
digitalWrite(int gpio, int value)
{
  printf ("GPIO%d <- %d\n", gpio, value);
}

unsigned
millis (void)
{
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void 
delay (unsigned int howLong)
{
  usleep (howLong * 1000);
}
