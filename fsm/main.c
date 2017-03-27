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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <wiringPi.h>
#include "fsm.h"

void timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b);
void timeval_add (struct timeval *res, struct timeval *a, struct timeval *b);
int timeval_less (struct timeval *a, struct timeval *b);

#define GPIO_BUTTON	2
#define GPIO_LED	3
#define GPIO_CUP	4
#define GPIO_COFFEE	5
#define GPIO_MILK	6

#define CUP_TIME	250
#define COFFEE_TIME	3000
#define MILK_TIME	3000

enum cofm_state {
  COFM_WAITING,
  COFM_CUP,
  COFM_COFFEE,
  COFM_MILK,
};

static int button = 0;
static void button_isr (void) { button = 1; }
static int button_pressed (fsm_t* this) { return button; }

static int timer = 0;
static void timer_isr (union sigval arg) { timer = 1; }
static struct timeval timer_endtime;
static void timer_start (int ms) { 
  struct timeval inc = { ms/1000, (ms % 1000) * 1000 };
  gettimeofday (&timer_endtime, NULL);
  timeval_add (&timer_endtime, &timer_endtime, &inc)
}
static int timer_finished (fsm_t* this) {
  struct timeval now;
  gettimeofday (&now, NULL);
  return timeval_less (&timer_endtime, &now); 
}


static void cup (fsm_t* this)
{
  digitalWrite (GPIO_LED, LOW);
  digitalWrite (GPIO_CUP, HIGH);
  timer_start (CUP_TIME);
  button = 0;
}

static void coffee (fsm_t* this)
{
  digitalWrite (GPIO_CUP, LOW);
  digitalWrite (GPIO_COFFEE, HIGH);
  timer_start (COFFEE_TIME);
}

static void milk (fsm_t* this)
{
  digitalWrite (GPIO_COFFEE, LOW);
  digitalWrite (GPIO_MILK, HIGH);
  timer_start (MILK_TIME);
}

static void finish (fsm_t* this)
{
  digitalWrite (GPIO_MILK, LOW);
  digitalWrite (GPIO_LED, HIGH);
}

/*
 * Explicit FSM description
 */
static fsm_trans_t cofm[] = {
  { COFM_WAITING, button_pressed, COFM_CUP,     cup    },
  { COFM_CUP,     timer_finished, COFM_COFFEE,  coffee },
  { COFM_COFFEE,  timer_finished, COFM_MILK,    milk   },
  { COFM_MILK,    timer_finished, COFM_WAITING, finish },
  {-1, NULL, -1, NULL },
};

/*
 * Utility functions, should be elsewhere
 */

/* res = a - b */
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

/* res = a + b */
void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_usec / 1000000 + b->tv_usec / 1000000; 
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

/* a < b */
int
timeval_less (struct timeval *a, struct timeval *b)
{
  return (a->tv_sec < b->tv_sec) ||
    ((a->tv_sec == b->tv_sec) && (a->tv_usec < b->tv_usec));
}

/* wait until next_activation (absolute time) */
void delay_until (struct timeval* next_activation)
{
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout);
}



int main ()
{
  struct timeval clk_period = { 0, 250 * 1000 };
  struct timeval next_activation;
  fsm_t* cofm_fsm = fsm_new (cofm);

  wiringPiSetup();
  pinMode (GPIO_BUTTON, INPUT);
  wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, button_isr);
  pinMode (GPIO_CUP, OUTPUT);
  pinMode (GPIO_COFFEE, OUTPUT);
  pinMode (GPIO_MILK, OUTPUT);
  pinMode (GPIO_LED, OUTPUT);
  digitalWrite (GPIO_LED, HIGH);
  
  gettimeofday (&next_activation, NULL);
  while (scanf("%d %d", &button, &timer) == 2) {
    fsm_fire (cofm_fsm);
    timeval_add (&next_activation, &next_activation, &clk_period);
    delay_until (&next_activation);
  }
}
