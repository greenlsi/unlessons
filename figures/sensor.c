#include "wiring.h"

char flag;

void sensorISR(){
	flag=1;
}

void sensor1_init(){
	flag=0;
	wiringPiISR (3, 0, sensorISR);
}

void *sensor1_task(){
	if(flag!=0){
		sensor1_process_data();
		flag=0;
	}
}
