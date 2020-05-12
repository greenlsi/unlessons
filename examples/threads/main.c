#include <stdio.h>
#include "task.h"
#include "model.h"
#include "interp.h"

static int setpoint;
static pthread_mutex_t m_setpoint;

void setpoint_set (int val)
{
	pthread_mutex_lock(&m_setpoint);
	setpoint = val;
	pthread_mutex_unlock(&m_setpoint);
}

int setpoint_get (void)
{
	int val;
	pthread_mutex_lock(&m_setpoint);
	val = setpoint;
	pthread_mutex_unlock(&m_setpoint);
	return val;
}

static void* heater_func (void* arg)
{
  struct timeval next_activation;
  struct timeval now, timeout;
  fsm_t* fsm = fsm_new_heater(setpoint_get, setpoint_set);
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    select (0, NULL, NULL, NULL, &timeout) ;

    fsm_fire (fsm);
  }
}

static char op;

int cmd_up (char* arg) { op = 'u'; return 0; }
int cmd_down (char* arg) { op = 'd'; return 0; }
int cmd_temp (char* arg) {
  extern int temp;
  if (*arg)
    temp = atoi(arg);
  else
    printf ("%d\n", temp);
  return 0;
}

static void* control_func (void* arg)
{
  struct timeval next_activation;
  struct timeval now, timeout;
  fsm_t* fsm = fsm_new_control(setpoint_get, setpoint_set, &op);
 
  interp_addcmd ("up", cmd_up, "increment setpoint");
  interp_addcmd ("down", cmd_down, "decrement setpoint");
  interp_addcmd ("temp", cmd_temp, "get/set temperature");

  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    select (0, NULL, NULL, NULL, &timeout) ;

    fsm_fire (fsm);
  }
}


int
main ()
{
  pthread_t t_heater, t_control;

  mutex_init (&m_setpoint, 2);
  t_heater  = task_new ("heater",  heater_func,  1000, 1000, 1, 1024);
  t_control = task_new ("control", control_func,  200,  200, 2, 1024);

  interp_run();
  return 0;
}
