#include <stdio.h>
#include "reactor.h"
#include "principal.h"
#include "codigo_alarma.h"
#include "timeval_helper.h"

fsm_t* principal;
fsm_t* codigo_alarma;

int boton_alarma;
int boton;
int presencia;
struct timeval espera_deadline1;

void scan_keyboard () {
  char buf[256];
  struct timeval timeout = {0,0};
  fd_set rdset;
  FD_ZERO (&rdset); 
  FD_SET (0, &rdset);
  if (select (1, &rdset, NULL, NULL, &timeout) > 0) {
    fgets (buf, 256, stdin);
    if ((buf[0])!='\n'){
      switch (buf[0]) {
        case 'a': 
          boton_alarma = 1;
          printf("boton alarma = 1\n"); 
          break;
        case 'b': 
          boton = 1; 
          printf("boton luz = 1\n");  
          break;
        case 'p': 
          presencia = 1; 
          printf("presencia = 1\n");  
          break;
      }
    }
  }
}

static void task_principal (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  fsm_fire (principal);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static void task_codigo_alarma (struct event_handler_t* this){
  static const struct timeval period = { 1, 0 };
  fsm_fire (codigo_alarma);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

int main (){
  static struct timeval period = { 1, 0 };
  struct timeval next;

  principal = fsm_new_principal();
  codigo_alarma = fsm_new_codigo_alarma();

  printf("\n");
  printf("\t****************************************\n\n");
  printf("\tTeclas de activación:\n\n");
  printf("\t\tPresencia: p\n");
  printf("\t\tBotón luz: b\n");
  printf("\t\tPulsador código alarma: a\n");
  printf("\t\tContraseña de la alarma: 2 1 1\n\n");
  printf("\t****************************************\n\n");

  EventHandler principal_eh, codigo_alarma_eh;
  reactor_init ();

  event_handler_init (&principal_eh, 2, task_principal);
  reactor_add_handler (&principal_eh);

  event_handler_init (&codigo_alarma_eh, 1, task_codigo_alarma);
  reactor_add_handler (&codigo_alarma_eh);

  while (1) {
    timeval_add (&next, &next, &period);
    scan_keyboard();
    reactor_handle_events();
  }
}
