#include <stdlib.h>
#include <wiringPi.h>
#include "tasks.h"
#include "sensor1.h"

sensor1_t* sensor1[NSENSOR1];
static int sensor1_event[NSENSOR1]; 
static pthread_t t_sensor1;

static void sensor1_isr_0 (void) { sensor1_event[0] = 1; }
static void sensor1_isr_1 (void) { sensor1_event[1] = 1; }
static void sensor1_isr_2 (void) { sensor1_event[2] = 1; }
static void sensor1_isr_3 (void) { sensor1_event[3] = 1; }

static void (*sensor1_isr[]) (void) = {
  sensor1_isr_0,
  sensor1_isr_1,
  sensor1_isr_2,
  sensor1_isr_3
};

static
void*
sensor1_task (void* arg)
{
  struct timeval next_activation;
  struct timeval now, timeout, rtime;
  int i;
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    timeval_sub (&rtime, period, &timeout);
    task_register_time (pthread_self(), &rtime);
    select (0, NULL, NULL, NULL, &timeout) ;

    for (i = 0; i < NSENSOR1; ++i) {
      if (sensor1_event[i]) {
        sensor1_event[i] = 0;
        sensor1_process_data (sensor1[i]);
      }
    }
  }
}

void sensor1_setup_sim (void);

void
sensor1_setup (int prio)
{
  int i;
  wiringPiSetup ();
  for (i = 0; i < NSENSOR1; ++i) {
    wiringPiISR (i, INT_EDGE_RISING, sensor1_isr[i]);
    sensor1[i] = sensor1_new (i);
  }
  t_sensor1 = task_new ("sensor1", sensor1_task, 500, 500, prio, 1024);
  sensor1_setup_sim ();
}

sensor1_t*
sensor1_new (int id)
{
  sensor1_t* this = (sensor1_t*) malloc (sizeof (sensor1_t));
  sensor1_init (this, id);
  return this;
}

void
sensor1_init (sensor1_t* this, int id)
{
  observable_init ((observable_t*) this);
  this->id = id;
}

void
sensor1_process_data (sensor1_t* this)
{
  observable_notify_observers ((observable_t*) this);
  /* ... */
}


