#include "tasks.h"
#include "sensor.h"


void *sensor_sim_task(){
	sensorISR();
}

void sensor_sim_init(){
	pthread_t t_sensor;
	create_task (&t_sensor, sensor_sim_task, NULL, 5000, 2, 1024);
}

