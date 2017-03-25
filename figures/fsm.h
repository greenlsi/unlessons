#ifndef FSM_H
#define FSM_H

#define MAXSTATES 10
#define MAXINS 32

struct fsm_t;

typedef void (*fsm_output_func_t) (struct fsm_t*);

typedef struct fsm_trans_t {
  int orig_state;
  int in;
  int dest_state;
  fsm_output_func_t run;
} fsm_trans_t;

typedef struct fsm_t {
  int current_state;
  fsm_trans_t* tt;
  /* optimized implementation */
  int next_state[MAXSTATES][MAXINS];
  fsm_output_func_t run[MAXSTATES][MAXINS];
} fsm_t;

fsm_t* fsm_new (fsm_trans_t* tt);
void fsm_init (fsm_t* this, fsm_trans_t* tt);
void fsm_fire (fsm_t* this, int in);

#endif
