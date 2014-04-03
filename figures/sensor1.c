#include <wiringPi.h>
#include "tasks.h"
#include "sensor1.h"

static int sensor1_event = 0; 
static pthread_t t_sensor1;

static
void
sensor1_isr (void)
{
  sensor1_event = 1;
}

static
void*
sensor1_task (void* arg)
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

    if (sensor1_event) {
      sensor1_event = 0;
      sensor1_process_data ();
    }
  }
}

void sensor1_setup_sim (void);

void
sensor1_setup (int prio)
{
  wiringPiSetup ();
  wiringPiISR (0, INT_EDGE_RISING, sensor1_isr);
  t_sensor1 = task_new ("sensor1", sensor1_task, 500, 500, prio, 1024);
  sensor1_setup_sim ();
}

void
sensor1_process_data (void)
{
  /* ... */
}


