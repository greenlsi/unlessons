#ifndef TIMEVAL_HELPER_H
#define TIMEVAL_HELPER_H

#include <sys/time.h>

int timeval_less (const struct timeval* a, const struct timeval* b);
void timeval_add (struct timeval* res,
             const struct timeval* a, const struct timeval* b);
void
timeval_sub (struct timeval* res,
             const struct timeval* a, const struct timeval* b);

#endif
