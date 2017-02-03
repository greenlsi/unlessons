#include <stdio.h>

void 
digitalWrite(int gpio, int value)
{
  printf ("GPIO%d <- %d\n", gpio, value);
}


