#ifndef OBSERVER_H
#define OBSERVER_H

#define MAXOBSERVERS 10

struct observable_t;
struct observer_t;

typedef void (*observer_notify_func_t) (struct observer_t*,
                                        struct observable_t*);

typedef struct observer_t {
  observer_notify_func_t notify;
} observer_t;

void observer_init (observer_t* this, observer_notify_func_t notify);
void observer_notify (observer_t* this, struct observable_t* observable);


typedef struct observable_t {
  observer_t* observer[MAXOBSERVERS];
  int nobservers;
} observable_t;

void observable_init (observable_t* this);
void observable_register_observer (observable_t* this, observer_t* observer);
void observable_notify_observers (observable_t* this);

#endif

