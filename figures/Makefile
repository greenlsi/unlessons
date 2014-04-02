CC=gcc
DEBUG='-DDEBUG(x)=x' -include stdio.h
CFLAGS=-Wall -g -O2 $(DEBUG)
LDLIBS=-lpthread -lreadline

all: main

test: main
	@for i in test/*.test ; do \
	  printf "%-30s" $$i ; \
	  LINES=24 COLUMNS=80 ./main < $$i 2>&1 > $$i.out ; \
	  if cmp $$i.out $$i.exp 2>&1 >/dev/null; then \
	    echo OK ; \
	  else \
	    echo FAIL ; \
	  fi ; \
	done

main: main.o tasks.o screen.o interp.o window.o

clean:
	$(RM) *.o *~ main *_test test/*.out

%_test.o: %.c
	$(CC) -DTEST $(CFLAGS) -c $< -o $@

.PHONY: test
