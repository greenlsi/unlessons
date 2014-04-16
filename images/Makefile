CC = gcc
CFLAGS = -Wall -Werror -g -pg -O2
LDFLAGS = -pg
CFLAGS_pg = -Wall -Werror -g -pg -O0
LDFLAGS_pg = -pg
CFLAGS_gcov = -Wall -Werror -g -fprofile-arcs -ftest-coverage -O
LDLIBS_gcov = -lgcov

all: process_pgm.gprof process_pgm.gcov process_pgm.cachegrind

%.gprof: %_pg.output
	gprof ./$*_pg 2>&1 | tee $@

%.gcov: %_gcov.output
	gcov ./$*_gcov 2>&1 | tee >$@

%.cachegrind: cachegrind.out.%
	cg_annotate --auto=yes $< 2>&1 | tee $@

cachegrind.out.%: %
	valgrind --tool=cachegrind --branch-sim=yes \
	  --cachegrind-out-file=$@ ./$* 2>&1 | tee $*.output

process_pgm_pg: process_pgm_pg.o
process_pgm_gcov: process_pgm_gcov.o
process_pgm: process_pgm.o

clean:
	rm -f process_pgm process_pgm_pg process_pgm_gcov
	rm -f horizontal_edges.pgm inverted_image.pgm
	rm -f thresholded_image.pgm vertical_edges.pgm
	rm -f *.gcno *.gcda *.gcov
	rm -f gmon.out *.gprof
	rm -f cachegrind.out.*
	rm -f *.output *.gprof *.gcov *.cachegrind
	rm -f *.o *~


%.output: %
	./$< 2>&1 > $@

%_pg: LDFLAGS = $(LDFLAGS_pg)
%_pg: LDLIBS = $(LDLIBS_pg)
%_pg.o: %.c
	$(CC) $(CFLAGS_pg) -o $@ -c $<

%_gcov: LDFLAGS = $(LDFLAGS_gcov)
%_gcov: LDLIBS = $(LDLIBS_gcov)
%_gcov.o: %.c
	$(CC) $(CFLAGS_gcov) -o $@ -c $<
