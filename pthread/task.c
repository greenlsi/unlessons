#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "task.h"

#define MAXTASKS 20

typedef struct taskdesk_t {
  pthread_t tid;
  const char* name;
  struct timeval period;
  struct timeval deadline;
  int prio;
} taskdesc_t;

static taskdesc_t desc[MAXTASKS];
static int ntasks = 0;

static
taskdesc_t*
taskdesc_find (pthread_t tid)
{
  int i;
  for (i = 0; i < ntasks; ++i)
    if (tid == desc[i].tid)
      return &desc[i];
  return NULL;
}

struct timeval*
task_get_period (pthread_t tid)
{
  taskdesc_t* this = taskdesc_find (tid);
  return &this->period;
}

struct timeval*
task_get_deadline (pthread_t tid)
{
  taskdesc_t* this = taskdesc_find (tid);
  return &this->deadline;
}

pthread_t
task_new (const char* name, void *(*f)(void *),
          int period_ms, int deadline_ms,
          int prio, int stacksize)
{
  taskdesc_t* tdesc = &desc[ntasks++];
  pthread_attr_t attr;
  struct sched_param sparam;
  sparam.sched_priority = sched_get_priority_min (SCHED_FIFO) + prio;

  pthread_attr_init (&attr);
  pthread_attr_setstacksize (&attr, stacksize);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
  pthread_attr_setschedparam (&attr, &sparam);
  pthread_create (&tdesc->tid, &attr, f, tdesc);

  tdesc->name = name;
  tdesc->prio = prio;
  tdesc->period.tv_sec = period_ms / 1000;
  tdesc->period.tv_usec = (period_ms % 1000) * 1000;
  tdesc->deadline.tv_sec = deadline_ms / 1000;
  tdesc->deadline.tv_usec = (deadline_ms % 1000) * 1000;

  return tdesc->tid;
}

void
mutex_init (pthread_mutex_t* m, int prioceiling)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  
  /* Only for priority ceiling protocol (not in Xenomai) */
  pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_PROTECT);

  /* Only for priority inheritance protocol */ 
  /* pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_INHERIT); */
  
  pthread_mutex_init (m, &attr);

  /* Only for priority ceiling protocol (not in Xenomai) */
  pthread_mutex_setprioceiling
    (m, sched_get_priority_min(SCHED_FIFO) + prioceiling, NULL);
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

int
timeval_less (struct timeval *a, struct timeval *b)
{
  return (a->tv_sec < b->tv_sec) ||
    ((a->tv_sec == b->tv_sec) && (a->tv_usec < b->tv_usec));
}

int
timeval_get_ms (struct timeval *a)
{
  return a->tv_sec * 1000 + a->tv_usec / 1000;
}

