#include <stdlib.h>
#include <wiringPi.h>
#include "fsm.h"

#define CLK_MS 100

#define GPIO_BUTTON1	2
#define GPIO_BUTTON2	3
#define GPIO_LIGHT	4

enum fsm_state {
  ENCENDIDO,
  APAGADO,
};

static int boton = 0;
static void boton_isr (void) { boton = 1; }

static int boton_pulsado (fsm_t* this) { return boton; }

static void encender (fsm_t* this) { boton = 0; digitalWrite (GPIO_LIGHT, 1); }
static void apagar   (fsm_t* this) { boton = 0; digitalWrite (GPIO_LIGHT, 0); }

/*
 * Máquina de estados: lista de transiciones
 * { EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
 */
static fsm_trans_t interruptor[] = {
  { APAGADO,   boton_pulsado, ENCENDIDO, encender },
  { ENCENDIDO, boton_pulsado, APAGADO,   apagar },
  {-1, NULL, -1, NULL },
};

/*
 * wait until next_activation (absolute time)
 */
void delay_until (unsigned int next)
{
  unsigned int now = millis();
  delay (next - now);
}

int main ()
{
  fsm_t* interruptor_fsm = fsm_new (interruptor);
  unsigned int next;

  wiringPiSetup();
  pinMode (GPIO_BUTTON1, INPUT);
  pinMode (GPIO_BUTTON2, INPUT);
  wiringPiISR (GPIO_BUTTON1, INT_EDGE_FALLING, boton_isr);
  wiringPiISR (GPIO_BUTTON2, INT_EDGE_FALLING, boton_isr);
  pinMode (GPIO_LIGHT, OUTPUT);
  apagar (interruptor_fsm);
  
  next = millis();
  while (1) {
    fsm_fire (interruptor_fsm);
    next += CLK_MS;
    delay_until (next);
  }
}
