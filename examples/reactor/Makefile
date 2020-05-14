CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDLIBS=-lpthread -lreadline

all: main

main: main.o reactor.o acceptor.o http.o fsm.o model.o task.o interp.o timeval_utils.o

clean:
	$(RM) *.o *~ main

