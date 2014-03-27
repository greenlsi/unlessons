#ifndef WINDOW_H
#define WINDOW_H

typedef struct window_t {
  char* text;
  int height;
  int width;
  int pos_x;
  int pos_y;
} window_t;

window_t* window_new (void);
void window_init (window_t* this);
void window_destroy (window_t* this);
void window_draw (window_t* this);



#endif
