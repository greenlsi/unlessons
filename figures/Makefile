CC=gcc
CFLAGS=-Wall -g -O2 $(DEFS)
LDLIBS=-lpthread -lreadline

all: main screen_test

main: main.o tasks.o screen.o interp.c

screen_test: screen_test.o tasks.o

clean:
	$(RM) *.o *~ main *_test

%_test.o: %.c
	$(CC) -DTEST $(CFLAGS) -c $< -o $@
