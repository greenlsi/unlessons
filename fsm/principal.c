#include "principal.h"
#include "timeval_helper.h"
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include "codigo_alarma.h"

extern int boton_alarma; // boton codigo alarma
extern int boton; // encender y apagar luz
extern int presencia; // encender luz y alarma
extern int alarma_EN;
static struct timeval t_ref;

static int transicion_01 (fsm_t* this) { 
	if (boton || (presencia&&(!alarma_EN))){
		return 1;
	else{
		return 0;
	}
}

static void enciende_luz (fsm_t* this) { 
	static struct timeval t_espera = { 60, 0 };
	gettimeofday (&t_ref, NULL);
	timeval_add (&t_ref, &t_ref, &t_espera);
	printf ("******* Luz ON *******\n"); 
	boton = 0;
	presencia=0;
}

static int transicion_10 (fsm_t* this) { 
	struct timeval now;
	gettimeofday (&now, NULL);
	timeout = timeval_less (&t_ref, &now); 
	if ( (boton&&(!presencia)) || timeout) {
		return 1;
	}
	else{
		return 0;
	}
}

static void apaga (fsm_t* this) {
	printf ("******* Luz OFF *******\n");
	printf("******* Sonido OFF *******\n");
	//Independientemente de que el sonido ya estuviese apagado
    boton = 0;
    presencia = 0;	
}

static int transicion_11 (fsm_t* this) {
	return presencia&&!alarma_EN;
}

static int transicion_12 (fsm_t* this) {
	return presencia&&alarma_EN;
}

static void enciende_sonido(fsm_t* this) {
	printf("******* Sonido ON *******\n");
	presencia=0;
	boton=0;
}

static int transicion_20 (fsm_t* this) {
	return !alarma_EN;
}

static void enciende_todo(fsm_t* this) {
	printf("******* Sonido ON *******\n");	
	printf ("******* Luz ON *******\n"); 
	presencia=0;
	boton=0;
}

fsm_t* fsm_new_luz ()
{
	static fsm_trans_t tt[] = {
		{ 0, transicion_01, 1, enciende_luz },
		{ 0, transicion_02, 2, enciende_todo},
		{ 1, transicion_10, 0, apaga},
		{ 1, transicion_11, 1, enciende_luz},
		{ 1, transicion_12, 2, enciende_sonido},
		{ 2, transicion_20, 0, apaga},
		{ -1, NULL, -1, NULL }
	};
	return fsm_new(tt);
}

