#include "greennode_defs.h"
// Archivo para funciones del PCB v1

void leds_init (void){
  INIT_GN_GLED();
  GN_GLED = 1;
  sleep(1);
  GN_GLED = 0;
  
  INIT_SW1();
  RLED_SW1 = 1;    // Retrieve current from sensors
  
  INIT_SW2();
  GLED_SW2 = 0;
  
  INIT_SW3();
  SELECT = 0; 
}
