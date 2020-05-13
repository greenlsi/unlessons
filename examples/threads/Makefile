CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDLIBS=-lreadline -lpthread

all: main

main: main.o task.o fsm.o model.o interp.o timeval_utils.o

clean:
	$(RM) *.o *~ main

