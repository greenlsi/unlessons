CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lwiringPi

all: main

main: main.o fsm.o

clean:
	$(RM) *.o *~ main libwiringPi.a

libwiringPi.a: wiringPi.o
	ar rcs $@ $^

