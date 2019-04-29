#include <sys/time.h>

int
timeval_less (const struct timeval* a, const struct timeval* b)
{
  return (a->tv_sec == b->tv_sec)? (a->tv_usec < b->tv_usec) :
    (a->tv_sec < b->tv_sec);
}
// timeval add hace a+b
void
timeval_add (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;
  if (res->tv_usec >= 1000000) {
    res->tv_sec += res->tv_usec / 1000000;
    res->tv_usec = res->tv_usec % 1000000;
  }
}

void
timeval_sub (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    res->tv_sec --;
    res->tv_usec += 1000000;
  }
}

