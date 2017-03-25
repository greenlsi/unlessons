#include <stdio.h>
#include "task.h"

static
void *
task1_func (void* arg)
{
  static int cnt = 0;

  struct timeval next_activation;
  struct timeval now, timeout;

  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    select (0, NULL, NULL, NULL, &timeout) ;

    printf ("%d\n", cnt++);
  }
}


int
main ()
{
  pthread_t tid = task_new ("task1", task1_func, 1000, 1000, 1, 1024);
  pthread_join (tid, NULL);
  return 0;
}
