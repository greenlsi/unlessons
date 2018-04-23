// based on https://github.com/drmackay/samplematrixcode.git

// A simple matrix multiply code to show affect of ordering and blocking
// to compile this use gcc -O2 mm.c -lrt or icc -O2 mm.c -lrt

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MEASURE_TIME(X) \
  { \
    struct timespec ts1, ts2; \
    clock_gettime (CLOCK_REALTIME, &ts1); \
    X; \
    clock_gettime (CLOCK_REALTIME, &ts2); \
    printf (#X " -> %f\n", (ts2.tv_sec - ts1.tv_sec) + (ts2.tv_nsec - ts1.tv_nsec) / 1.0e9); \
  }


void setmat (int n, int m, double* a)
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0 ; j < m; j++)
      a[i * n + j] = 0.0 ;
}

void fillmat (int n, int m, double* a)
{
   int i, j;

   for (i = 0; i < n; i++)
      for (j = 0 ; j < m; j++)
         a[i * n + j] = (double)rand() / 3.1e09;
}

void mm_basic (int n, double* a, double* b, double* c)
{
   int i, j, k;

   for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
         for (k = 0; k < n; k++)
            a[i * n + j] += b[i * n + k] * c[k * n + j] ;
}


void checkmm (int n, double* a, double* aa)
{
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if (a[i * n + j]-aa[i * n + j] != 0.0) {
	printf ("(%d, %d) -> diffi: %lf\n", i, j, a[i * n + j]-aa[i * n + j]);
	return;
      }
    }
  printf ("check OK\n");
}

int main (int argc, char *argv[])
{
  double *aa, *a, *b, *c;
  unsigned int n, blockSize;


  if (argc != 3) {
    printf ("Usage: %s <matrix_size> <blocksize>\n", argv[0]) ;
    exit (1);
  }
  n = atoi (argv[1]) ;
  blockSize = atoi (argv[2]) ;

  printf ("matrix size (%d x %d), blocksize (%d, %d)\n", n, n, blockSize, blockSize) ;
  if (n % blockSize) {
    printf ("matrix size should be multiple of blocksize\n") ;
    exit (2);
  }
   
  // allocate matrices
   a =  (double*) calloc (n*n, sizeof(double)) ;
   b =  (double*) calloc (n*n, sizeof(double)) ;
   c =  (double*) calloc (n*n, sizeof(double)) ;
   aa = (double*) calloc (n*n, sizeof(double)) ;
   if (aa == NULL) {
     printf ("insufficient memory \n");
     exit(3);
   }

  // fill a matrix
  setmat (n, n, a) ;
  setmat (n, n, aa) ;

  srand (416) ; // set random seed (change to go off time stamp to make it better

  fillmat (n, n, b) ;
  fillmat (n, n, c) ;

  MEASURE_TIME ( mm_basic (n, aa, b, c) );

  checkmm (n, a, aa) ;
}
   

