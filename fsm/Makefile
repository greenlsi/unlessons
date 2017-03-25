CC=gcc
CFLAGS=-g -Wall -ansi -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lwiringPi

all: libwiringPi.a main interruptor

main: main.o fsm.o
interruptor: interruptor.o fsm.o

clean:
	$(RM) *.o *~ main interruptor libwiringPi.a

libwiringPi.a: wiringPi.o
	ar rcs $@ $^

