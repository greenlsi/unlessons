#include <unistd.h>
#include <stdlib.h>
#include "tasks.h"
#include "screen.h"
#include "window.h"
#include "interp.h"

static int deltax = 1;
static int deltay = 0;

static int cmd_x (char* arg)
{
  deltax = atoi (arg);
  return 0;
}

static int cmd_y (char* arg)
{
  deltay = atoi (arg);
  return 0;
}

static
void*
snake (void* arg)
{
  static int x = 3;
  static int y = 3;
  
  struct timeval next_activation;
  struct timeval now, timeout, rtime;

  interp_addcmd ("x", cmd_x, "Set x increment");
  interp_addcmd ("y", cmd_y, "Set y increment");
  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    timeval_sub (&rtime, period, &timeout);
    task_register_time (pthread_self(), &rtime);
    select (0, NULL, NULL, NULL, &timeout) ;

    x += deltax;
    y += deltay;
    screen_printxy (x, y, "*");
  }
}

int
main (void)
{
  task_setup();
  screen_init (2);
  window_setup();
  
  task_new (snake, 500, 500, 2, 1024);

  interp_run ();

  exit (0);
}
