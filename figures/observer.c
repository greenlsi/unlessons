#include "observer.h"

void
observer_init (observer_t* this, observer_notify_func_t notify)
{
  this->notify = notify;
}

void
observer_notify (observer_t* this, observable_t* observable)
{
  this->notify (this, observable);
}


void
observable_init (observable_t* this)
{
  this->nobservers = 0;
}

void
observable_register_observer (observable_t* this, observer_t* obs)
{
  this->observer[this->nobservers++] = obs;
}

void
observable_notify_observers (observable_t* this)
{
  int i;
  for (i = 0; i < this->nobservers; ++i)
    observer_notify (this->observer[i], this);
}

