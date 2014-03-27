#include <pthread.h>
#include <time.h>
#include <sys/time.h>

void
create_task (pthread_t* tid, void *(*f)(void *), void* arg,
             int period_ms, int prio, int stacksize)
{
  pthread_attr_t attr;
  struct sched_param sparam;
  sparam.sched_priority = sched_get_priority_min (SCHED_FIFO) + prio;

  pthread_attr_init (&attr);
  pthread_attr_setstacksize (&attr, stacksize);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
  pthread_attr_setschedparam (&attr, &sparam);
  pthread_create (tid, &attr, f, arg);
}

void
init_mutex (pthread_mutex_t* m, int prioceiling)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  //pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_PROTECT);
  pthread_mutex_init (m, &attr);
  //pthread_mutex_setprioceiling
  //  (m, sched_get_priority_min(SCHED_FIFO) + prioceiling, NULL);
}


void
timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    --res->tv_sec;
    res->tv_usec += 1000000;
  }
}

void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_usec / 1000000 + b->tv_usec / 1000000;
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}
