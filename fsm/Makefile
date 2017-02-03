CC=gcc
CFLAGS=-D_POSIX_SOURCE -g -Wall -ansi -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lwiringPi

all: main interruptor

main: main.o fsm.o
interruptor: interruptor.o fsm.o

clean:
	$(RM) *.o *~ main libwiringPi.a

libwiringPi.a: wiringPi.o
	ar rcs $@ $^

