#ifndef OBSERVER_H
#define OBSERVER_H

#define MAXOBSERVERS 10

typedef struct observer_t {
  void (*notify) (struct observer_t*);
} observer_t;

void observer_init (observer_t* this, void (*notify)(observer_t*));
void observer_notify (observer_t* this);


typedef struct observable_t {
  observer_t* observer[MAXOBSERVERS];
  int nobservers;
} observable_t;

void observable_init (observable_t* this);
void observable_register_observer (observable_t* this, observer_t* obs);
void observable_notify_observers (observable_t* this);

#endif

