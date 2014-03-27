#include <stdlib.h>
#include "tasks.h"
#include "sensor.h"


static int deltax = 1;
static int deltay = 0;

static
void*
snake (void* arg)
{
  static int x = 3;
  static int y = 3;
  
  struct timeval next_activation;
  struct timeval now, timeout;
  struct timeval period = { 0, 500000 };
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    select (0, NULL, NULL, NULL, &timeout) ;
    timeval_add (&next_activation, &next_activation, &period);

    x += deltax;
    y += deltay;
    screen_printxy (x, y, "*");
  }
}

void interp_run(void);

int
main (void)
{
  //pthread_t t_snake;
  //void* ret;
	sensor1_init();
sensor_sim_task();

  //screen_init (2);

  //create_task (&t_snake, snake, NULL, 500, 2, 1024);

  interp_run ();
  //pthread_join(t_snake, &ret);
  return 0;
}
