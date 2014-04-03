#include <stdlib.h>
#include "fsm.h"

fsm_t*
fsm_new (fsm_trans_t* tt)
{
  fsm_t* this = (fsm_t*) malloc (sizeof (fsm_t));
  fsm_init (this, tt);
  return this;
}

void
fsm_init (fsm_t* this, fsm_trans_t* tt)
{
  int state, in;
  this->tt = tt;
  for (state = 0; state < MAXSTATES; ++state) {
    for (in = 0; in < MAXINS; ++in) {
      this->next_state[state][in] = state;
      this->run[state][in] = NULL;
    }
  }
  for (; tt->orig_state >= 0; ++tt) {
      this->next_state[tt->orig_state][tt->in] = tt->dest_state;
      this->run[tt->orig_state][tt->in] = tt->run;
  }
}

void
fsm_fire (fsm_t* this, int in)
{
  fsm_output_func_t run = this->run[this->current_state][in];
  this->current_state = this->next_state[this->current_state][in];
  if (run)
    run (this);
}

