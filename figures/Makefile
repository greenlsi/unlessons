CC = gcc
CFLAGS = $(SYS_CFLAGS) -Wall -g -O2 $(DEBUG)
LDLIBS = $(SYS_LDLIBS) -lreadline

# debug options
DEBUG='-DDEBUG(x)=x' -include stdio.h

# native RaspberryPi
#SYS_CFLAGS = $(shell xeno-config --skin posix --cflags)
#SYS_LDLIBS = $(shell xeno-config --skin posix --ldflags)

# simulated environmet
SYS_CFLAGS = -I.
SYS_LDLIBS = -lpthread
SIM_OBJS = wiringPi.o sensor1_sim.o

all: main

test: main
	@for i in test/*.test ; do \
	  printf "%-30s" $$i ; \
	  LINES=24 COLUMNS=80 ./main < $$i 2>&1 > $$i.out ; \
	  if cmp $$i.out $$i.exp 2>&1 >/dev/null; then \
	    echo PASS ; \
	  else \
	    echo FAIL ; \
	  fi ; \
	done

main:	main.o task.o screen.o interp.o window.o \
	observer.o fsm.o sensor1.o actuator1.o $(SIM_OBJS)

clean:
	$(RM) *.o *~ main *_test test/*.out

%_test.o: %.c
	$(CC) -DTEST $(CFLAGS) -c $< -o $@

.PHONY: test
