/* FSM example

   Copyright (C) 2017 by Jose M. Moya

   This file is part of GreenLSI Unlessons.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
