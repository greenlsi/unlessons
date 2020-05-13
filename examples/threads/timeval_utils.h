#ifndef TIMEVAL_UTILS_H
#define TIMEVAL_UTILS_H

int timeval_less (const struct timeval* a, const struct timeval* b);
void timeval_add (struct timeval* res, const struct timeval* a, const struct timeval* b);
void timeval_sub (struct timeval* res, const struct timeval* a, const struct timeval* b);

#endif
