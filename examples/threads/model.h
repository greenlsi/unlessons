#include "fsm.h"

typedef int (*setpoint_get_t) (void);
typedef void (*setpoint_set_t) (int);

fsm_t* fsm_new_heater  (setpoint_get_t get, setpoint_set_t set);
fsm_t* fsm_new_control (setpoint_get_t get, setpoint_set_t set, char* op);
