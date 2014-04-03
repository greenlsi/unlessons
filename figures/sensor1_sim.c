#include "wiringPi.h"
#include "tasks.h"

static pthread_t t_sensor1_sim;

static
void*
sensor1_sim (void* arg)
{
  struct timeval next_activation;
  struct timeval now, timeout, rtime;
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    timeval_sub (&rtime, period, &timeout);
    task_register_time (pthread_self(), &rtime);
    select (0, NULL, NULL, NULL, &timeout) ;

    wiringPi_gen_interrupt (0);
  }
}

void
sensor1_setup_sim (void)
{
  t_sensor1_sim = task_new ("s1sim", sensor1_sim, 500, 500, 1, 1024);
}
