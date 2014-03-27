#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "window.h"
#include "screen.h"

window_t*
window_new (void)
{
  window_t* this = (window_t*) malloc (sizeof (window_t));
  window_init (this);
  return this;
}

void
window_init (window_t* this)
{
  this->text = "Window";
  this->pos_x = this->pos_y = 3;
  this->width = 20;
  this->height = 10;
  window_draw (this);
}

void
window_destroy (window_t* this)
{
  free (this);
}


void
window_draw (window_t* this)
{
  int buc_x, buc_y;
  
  for (buc_y=0 ; buc_y < this->height ; buc_y++) {
    for (buc_x=0 ; buc_x < this->width ; buc_x++) {
      screen_printxy(buc_x, buc_y, "@");
    }
  }
}

