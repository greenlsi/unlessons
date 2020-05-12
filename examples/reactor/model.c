#include "model.h"
#include "task.h"
#include "timeval_utils.h"
#include <stdlib.h>
#include <stdio.h>

struct fsm_heater_t {
	struct fsm_t fsm;
	setpoint_get_t get;
	setpoint_set_t set;
	struct timeval end;
};
typedef struct fsm_heater_t fsm_heater_t;

static int temp;

int temp_low (fsm_t* this) {
	fsm_heater_t* fsm = (fsm_heater_t*) this;
	int val = fsm->get();
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less(&fsm->end, &now) && (temp < val);
}

int temp_high (fsm_t* this) {
	fsm_heater_t* fsm = (fsm_heater_t*) this;
	int val = fsm->get();
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less(&fsm->end, &now) && (temp > val);
}

void heat_start (fsm_t* this) {
	struct timeval timeout = { 30, 0 };
	fsm_heater_t* fsm = (fsm_heater_t*) this;
	printf ("\ncalentando\n");
	gettimeofday (&fsm->end, NULL);
	timeval_add (&fsm->end, &fsm->end, &timeout);
}

void heat_stop (fsm_t* this) {
	struct timeval timeout = { 120, 0 };
	fsm_heater_t* fsm = (fsm_heater_t*) this;
	printf ("stop\n");
	gettimeofday (&fsm->end, NULL);
	timeval_add (&fsm->end, &fsm->end, &timeout);
}


fsm_t* fsm_new_heater  (setpoint_get_t get, setpoint_set_t set)
{
	static fsm_trans_t tt[] = {
		{ 0, temp_low, 1, heat_start },
		{ 1, temp_low, 1, heat_start },
		{ 1, temp_high, 0, heat_stop },
		{ -1, NULL, -1, NULL },
	};
	fsm_heater_t* fsm = (fsm_heater_t*) malloc (sizeof (fsm_heater_t));
	fsm_init ((fsm_t*) fsm, tt);
	fsm->get = get;
	fsm->set = set;
	gettimeofday (&fsm->end, NULL);
	return (fsm_t*) fsm;
}

struct fsm_control_t {
	struct fsm_t fsm;
	setpoint_get_t get;
	setpoint_set_t set;
	char* op;
};
typedef struct fsm_control_t fsm_control_t;

int up (fsm_t* this) {
        fsm_control_t* fsm = (fsm_control_t*) this;
        return *fsm->op == 'u';
}

int down (fsm_t* this) {
        fsm_control_t* fsm = (fsm_control_t*) this;
        return *fsm->op == 'd';
}

void setpoint_inc (fsm_t* this) {
        fsm_control_t* fsm = (fsm_control_t*) this;
	fsm->set(fsm->get() + 1);
}

void setpoint_dec (fsm_t* this) {
        fsm_control_t* fsm = (fsm_control_t*) this;
	fsm->set(fsm->get() - 1);
}

fsm_t* fsm_new_control (setpoint_get_t get, setpoint_set_t set, char* op)
{
	static fsm_trans_t tt[] = {
		{ 0, up,   0, setpoint_inc },
		{ 0, down, 0, setpoint_dec },
		{ -1, NULL, -1, NULL },
	};
	fsm_control_t* fsm = (fsm_control_t*) malloc (sizeof (fsm_control_t));
	fsm_init ((fsm_t*) fsm, tt);
	fsm->get = get;
	fsm->set = set;
	fsm->op = op;
	return (fsm_t*) fsm;
}
