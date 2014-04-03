#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include "screen.h"
#include "tasks.h"

static
void*
refresh_screen (void* arg)
{
  struct timeval next_activation;
  struct timeval now, timeout, rtime;
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    struct timeval *period = task_get_period (pthread_self());
    timeval_add (&next_activation, &next_activation, period);
    gettimeofday (&now, NULL);
    timeval_sub (&timeout, &next_activation, &now);
    timeval_sub (&rtime, period, &timeout);
    task_register_time (pthread_self(), &rtime);
    select (0, NULL, NULL, NULL, &timeout) ;

    screen_refresh();
  }
}


static struct termios oldtc, newtc;
static char* screen;
static int columns;
static int lines;

static pthread_t t_screen;
static pthread_mutex_t m_scr;

static char*
scr (int x, int y)
{
  if (x >= columns)
    x = columns - 1;
  if (y >= lines)
    y = lines - 1;
  return screen + y * (columns + 1) + x;
}

static
int getenv_int (const char *var, int defval)
{
  char* val = getenv (var);
  return val? atoi(val) : defval;
}

void
screen_setup (int prio)
{
  columns = getenv_int ("COLUMNS", 80);
  lines = getenv_int ("LINES", 24) / 2;

  mutex_init (&m_scr, prio);
  screen = (char *) malloc ((columns + 1) * lines);
  screen_clear ();

  printf ("\e[2J\e[%d;1f", lines + 1);
  fflush (stdout);

  tcgetattr(0, &oldtc);
  newtc = oldtc;
  newtc.c_lflag &= ~ICANON;
  newtc.c_lflag |= ECHO;
  tcsetattr(0, TCSANOW, &newtc);

  t_screen = task_new ("screen", refresh_screen, 500, 500, 1, 1024);
}

void
screen_refresh (void)
{
  int y;

  printf ("\e7\e[?25l");

  pthread_mutex_lock (&m_scr);
  for (y = 0; y < lines; ++y) {
    printf ("\e[%d;1f%s", y+1, scr(0,y));
  }
  pthread_mutex_unlock (&m_scr);

  printf ("\e8\e[?25h");
  fflush (stdout);
}

void
screen_clear (void)
{
  int y;

  pthread_mutex_lock (&m_scr);
  memset (screen, ' ', (columns + 1) * lines);
  for (y = 0; y < lines; ++y)
    *scr(columns, y) = '\0';
  pthread_mutex_unlock (&m_scr);
}

void
screen_printxy (int x, int y, const char* txt)
{
  char* p = scr(x,y); 
  pthread_mutex_lock (&m_scr);
  while (*txt) {
    *p++ = *txt++;
  }
  pthread_mutex_unlock (&m_scr);
}

int
screen_getchar (void)
{
  fd_set rds;
  struct timeval t = {0, 0};
  int ch = 0;
  
  FD_ZERO (&rds);
  FD_SET (0, &rds);
  pthread_mutex_lock (&m_scr);
  if (select (1, &rds, NULL, NULL, &t) > 0) {
    ch = getchar();
  }
  pthread_mutex_unlock (&m_scr);
  return ch;
}


#ifdef TEST

int
main ()
{
  screen_init (1);
  return 0;
}

#endif
