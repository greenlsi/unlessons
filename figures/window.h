#ifndef WINDOW_H
#define WINDOW_H

typedef struct window_t {
  int x;
  int y;
  int width;
  int height;
  char* text;
} window_t;

void window_setup (void);
void window_redraw (void);

window_t* window_new (void);
void window_init (window_t* this);
void window_destroy (window_t* this);
void window_draw (window_t* this);
void window_move (window_t* this, int x, int y);
void window_resize (window_t* this, int w, int h);
void window_text (window_t* this, char* txt);
void window_order (window_t* this, int pos);

#endif
