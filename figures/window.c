#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "window.h"
#include "screen.h"
#include "interp.h"

#define MAXWINDOWS 100

static window_t* win[MAXWINDOWS];
static int current_window = 0;

static int
cmd_window (char* arg)
{
  if (0 == strcmp (arg, "new")) {
    window_new ();
    return 0;
  }
  if (0 == strcmp (arg, "list")) {
    int i;
    for (i = 0; win[i]; ++i) {
      printf ("%d%c\t%s\n", i, (i == current_window)? '*' : ' ', win[i]->text);
    }
    return 0;
  }
  if (0 == strcmp (arg, "info")) {
    window_t* this = win[current_window];
    printf ("window %d:\n", current_window);
    printf ("  (x, y) = (%d, %d)\n", this->x, this->y);
    printf ("  size = %d x %d\n", this->width, this->height);
    printf ("  text = %s\n", this->text);
    return 0;
  }
  if (0 == strncmp (arg, "select ", strlen("select "))) {
    current_window = atoi (arg + strlen("select "));
    return 0;
  }
  if (0 == strncmp (arg, "move ", strlen("move "))) {
    int x, y;
    sscanf (arg + strlen("move "), "%d %d", &x, &y);
    window_move (win[current_window], x, y);
    return 0;
  }
  if (0 == strncmp (arg, "size ", strlen("size "))) {
    int w, h;
    sscanf (arg + strlen("size "), "%d %d", &w, &h);
    window_resize (win[current_window], w, h);
    return 0;
  }
  if (0 == strncmp (arg, "text ", strlen("text "))) {
    window_text (win[current_window], arg + strlen("text "));
    return 0;
  }
  if (0 == strncmp (arg, "order ", strlen("order "))) {
    window_order (win[current_window], atoi (arg + strlen("order ")));
    return 0;
  }
  return 1;
}

void
window_setup (void)
{
  interp_addcmd
    ("window", cmd_window,
     "window { new | list | info | select <n> | move <x> <y> | size <w> <h> | text <txt> }");
  win[0] = NULL;
}

void
window_redraw (void)
{
  int i;
  screen_clear ();
  for (i = 0; win[i]; ++i) {
    window_draw (win[i]);
  }
}


window_t*
window_new (void)
{
  int i;
  for (i = 0; i < (MAXWINDOWS - 1); ++i) {
    if (win[i] == NULL) {
      window_t* this = (window_t*) malloc (sizeof (window_t));
      window_init (this);
      win[i] = this;
      win[++i] = NULL;
      return this;
    }
  }
  return NULL;
}

void
window_init (window_t* this)
{
  this->x = this->y = 3;
  this->width = 20;
  this->height = 10;
  this->text = (char*) malloc ((this->width - 2) * (this->height - 2));
  strcpy (this->text, "Window");
  window_draw (this);
}

void
window_destroy (window_t* this)
{
  free (this->text);
  free (this);
}


void
window_draw (window_t* this)
{
  int x, y;

  screen_printxy (this->x, this->y, "+");
  screen_printxy (this->x + this->width - 1, this->y, "+");
  screen_printxy (this->x, this->y + this->height - 1, "+");
  screen_printxy (this->x + this->width - 1, this->y + this->height - 1, "+");

  for (x = 1; x < (this->width - 1); ++x) {
      screen_printxy (this->x + x, this->y, "-");
      screen_printxy (this->x + x, this->y + this->height - 1, "-");
  }
  for (y = 1; y < (this->height - 1); y++) {
      screen_printxy (this->x, this->y + y, "|");
      screen_printxy (this->x + this->width - 1, this->y + y, "|");
  }
  for (x = 1; x < (this->width - 1); ++x) {
    for (y = 1; y < (this->height - 1); y++) {
      screen_printxy (this->x + x, this->y + y, " ");
    }
  }
  screen_printxy(this->x + 1, this->y + 1, this->text);
}

void
window_move (window_t* this, int x, int y)
{
  this->x = x;
  this->y = y;
  window_redraw ();
}

void
window_resize (window_t* this, int w, int h)
{
  this->width = w;
  this->height = h;
  this->text = (char*) realloc (this->text, (w - 2) * (h - 2));
  window_redraw ();
}

void
window_text (window_t* this, char* txt)
{
  strcpy (this->text, txt);
  window_redraw ();
}

void
window_order (window_t* this, int pos)
{
  int i, j, current;
  for (i = 0; win[i]; ++i) {
    if (win[i] == this) {
      current = i;
      if (current < pos) {
        for (j = current; j < pos; ++j)
          win[j] = win[j + 1];
        win[pos] = this;
      }
      else {
        for (j = pos; j < current; ++j)
          win[j + 1] = win[j];
        win[pos] = this;
      }
      window_redraw ();
      return;
    }
  } 
}

#ifdef TEST

int
main ()
{
  screen_init (1);
  window_setup ();
  return 0;
}

#endif
