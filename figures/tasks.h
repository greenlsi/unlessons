#ifndef TASKS_H
#define TASKS_H

#include <pthread.h>
#include <time.h>
#include <sys/time.h>

void create_task (pthread_t* tid, void *(*f)(void *), void* arg,
                  int period_ms, int prio, int stacksize);
void init_mutex (pthread_mutex_t* m, int prioceiling);

void timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b);
void timeval_add (struct timeval *res, struct timeval *a, struct timeval *b);

#endif

