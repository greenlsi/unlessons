#include "greennode_defs.h"

/// Archivo para funciones del PCB v2
void leds_init () {
  INIT_LED_SW2();   /* Define LED_SW2 port as output */
  LED_SW2 = 1;	    /* LED=ON */
  sleep(1);
  LED_SW2 = 0;      /* LED=OFF */
  
  INIT_SW1();	    /* Define SW1 port as output */
  SW1 = 1;          /* Remove current to sensors. PMOS1=OFF */
}
