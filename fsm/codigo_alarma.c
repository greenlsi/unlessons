#include "timeval_helper.h"
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include "codigo_alarma.h"

extern int boton_alarma; // variable compartida
extern int alarma_EN;
char clave[3] = {2,1,1};  //la clave que debe ser introducida correctamente
char codigo[3];
char indice = 0;
int count = 0;
struct timeval referencia;

static int transicion_01 (fsm_t* this) {
	return boton_alarma;
}

static void salida_01 (fsm_t* this) { 
	gettimeofday (&referencia, NULL);
	static struct timeval t_espera = { 3, 0 };
	timeval_add (&referencia, &referencia, &t_espera);
	count++;
}

static int transicion_22a (fsm_t* this) {
	gettimeofday(&now, NULL);
	check = timeval_less(&referencia, &now); 
	return boton_alarma&&(!check);
}

static void salida_22a (fsm_t* this) { 
	count++;
	//delay(200)//???? como se hace aqui? para filtrar rebotes
}

static int transicion_22b (fsm_t* this) {
	gettimeofday(&now, NULL);
	check = timeval_less(&referencia, &now);
	return boton_alarma&&check;
}

static void salida_22b (fsm_t* this) {
	if (count>=10){codigo[indice]=0;}
	else{ codigo[indice]=count;}
	indice++;
	count=0;
}

static int transicion_23 (fsm_t* this) {
	if (indice >= 3) {return 1;}
	else{ return 0;}
}

static void salida_23 (fsm_t* this) {
	//no hago nada
}

static int transicion_30 (fsm_t* this) {
	if ((codigo[0]==clave[0])&&(codigo[1]==clave[1])&&(codigo[2]==clave[2])){
		alarma_EN = !alarma_EN;
	}
	return true;
}

fsm_t* fsm_new_alarma ()
{
	static fsm_trans_t tt[] = {
		{ 0, transicion_01, 1, salida_01 },
		{ 2, transicion_22a, 2, salida_22a},
		{ 2, transicion_22b, 2, salida_22b},
		{ 2, transicion_23, 3, salida_23},
		{ -1, NULL, -1, NULL }
	};
	return fsm_new (tt);
}