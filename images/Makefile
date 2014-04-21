CC = gcc
CFLAGS = -Wall -Werror -g -pg -O0
LDFLAGS = -pg
CFLAGS_pg = -Wall -Werror -g -pg -O2
LDFLAGS_pg = -pg
CFLAGS_gcov = -Wall -Werror -g -fprofile-arcs -ftest-coverage -O
LDLIBS_gcov = -lgcov

all: process_pgm_pg.output process_pgm_gcov.output process_pgm.output

process_pgm_pg: process_pgm_pg.o
process_pgm_gcov: process_pgm_gcov.o
process_pgm: process_pgm.o

clean:
	rm -f process_pgm process_pgm_pg process_pgm_gcov
	rm -f horizontal_edges.pgm inverted_image.pgm thresholded_image.pgm vertical_edges.pgm
	rm -f *.o *~


%.output: %
	$< 2>&1 > $@

%_pg.o: %.c
	$(CC) $(LDFLAGS_pg) -o $@ $^ $(LDLIBS_pg)

%_gcov.o: %.c
	$(CC) $(LDFLAGS_gcov) -o $@ $^ $(LDLIBS_gcov)
