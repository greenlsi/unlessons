#include <sys/select.h>
#include <sys/time.h>
#include <wiringPi.h>
#include "fsm.h"

#define GPIO_BUTTON	2
#define GPIO_LED	3
#define GPIO_CUP	4
#define GPIO_COFFEE	5
#define GPIO_MILK	6

enum mcafe_state {
  MCAFE_WAITING,
  MCAFE_CUP,
  MCAFE_COFFEE,
  MCAFE_MILK,
};

static int button = 0;
static int timer = 0;

static int button_pressed (fsm_t* this)
{
  int ret = button;
  button = 0;
  return ret;
}

static int timer_finished (fsm_t* this)
{
  int ret = timer;
  timer = 0;
  return ret;
}

static void cup (fsm_t* this)
{
  digitalWrite (GPIO_LED, LOW);
  digitalWrite (GPIO_CUP, HIGH);
}

static void coffee (fsm_t* this)
{
  digitalWrite (GPIO_CUP, LOW);
  digitalWrite (GPIO_COFFEE, HIGH);
}

static void milk (fsm_t* this)
{
  digitalWrite (GPIO_COFFEE, LOW);
  digitalWrite (GPIO_MILK, HIGH);
}

static void finish (fsm_t* this)
{
  digitalWrite (GPIO_MILK, LOW);
  digitalWrite (GPIO_LED, HIGH);
}



static fsm_trans_t mcafe[] = {
  { MCAFE_WAITING, button_pressed, MCAFE_CUP,     cup    },
  { MCAFE_CUP,     timer_finished, MCAFE_COFFEE,  coffee },
  { MCAFE_COFFEE,  timer_finished, MCAFE_MILK,    milk   },
  { MCAFE_MILK,    timer_finished, MCAFE_WAITING, finish },
  {-1, NULL, -1, NULL },
};


// Utility functions, should be elsewhere

// res = a - b
void
timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    --res->tv_sec;
    res->tv_usec += 1000000;
  }
}

// res = a + b
void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_usec / 1000000 + b->tv_usec / 1000000; 
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

// wait until next_activation (absolute time)
void delay_until (struct timeval* next_activation)
{
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout) ;
}



int main ()
{
  struct timeval clk_period = { 0, 250000 };
  struct timeval next_activation;
  fsm_t* mcafe_fsm = fsm_new (mcafe);

  wiringPiSetup();
  pinMode (GPIO_BUTTON, INPUT);
  pinMode (GPIO_CUP, OUTPUT);
  pinMode (GPIO_COFFEE, OUTPUT);
  pinMode (GPIO_MILK, OUTPUT);
  pinMode (GPIO_LED, OUTPUT);
  digitalWrite (GPIO_LED, HIGH);
  
  gettimeofday (&next_activation, NULL);
  while (1) {
    fsm_fire (mcafe_fsm);
    timeval_add (&next_activation, &next_activation, &clk_period);
    delay_until (&next_activation);
  }
}
