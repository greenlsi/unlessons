#include <stdlib.h>
#include "wiringPi.h"
#include "tasks.h"
#include "sensor1.h"

static pthread_t t_sensor1_sim;

static
void*
sensor1_sim (void* arg)
{
  static int i = 0;
  struct timeval timeout;
  
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeout.tv_sec = period->tv_sec;
    timeout.tv_usec = period->tv_usec
      + rand() % (period->tv_usec / 2)
      - (period->tv_usec / 4);
    select (0, NULL, NULL, NULL, &timeout) ;

    wiringPi_gen_interrupt (i);
    i = (i + 1) % NSENSOR1;
  }
}

void
sensor1_setup_sim (void)
{
  t_sensor1_sim = task_new ("s1sim", sensor1_sim, 1500, 1500, 1, 1024);
}
