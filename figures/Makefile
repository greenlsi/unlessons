CC=gcc
DEBUG='-DDEBUG(x)=x' -include stdio.h
CFLAGS=-Wall -g -O2 $(DEBUG)
LDLIBS=-lpthread -lreadline

all: main screen_test

main: main.o tasks.o screen.o interp.o window.o

screen_test: screen_test.o tasks.o

clean:
	$(RM) *.o *~ main *_test

%_test.o: %.c
	$(CC) -DTEST $(CFLAGS) -c $< -o $@
