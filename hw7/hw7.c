/*  Edited by:   Vincent T. Mossman
	Programmer:  Mark Fienup
    File:        hw7.c
    Compiled by: gcc -o sor -O3 hw7.c -lpthread -lm
    Run by:      ./sor 1000 0.00001 8
    Description:  2D SOR (successive over-relaxation) program written using POSIX threads.
*/
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include "timer.h"

#define MAXTHREADS 16	/* Assume max. # threads */
#define TRUE 1
#define FALSE 0
#define BOOL int

double ** allocate2DArray(int rows, int columns);
void print2DArray(int rows, int columns, double ** array2D);
BOOL equal2DArrays(int rows, int columns, double ** array1, double ** array2,
		   double tolerance);
void * thread_main(void *);
void initializeData(double ** val, int n);
void sequential2D_SOR();

/* BARRIER prototype, mutex, condition variable, if needed */
void barrier();
pthread_mutex_t update_lock;
pthread_mutex_t barrier_lock;	/* mutex for the barrier */
pthread_cond_t all_here;	/* condition variable for barrier */
int count=0;			/* counter for barrier */

/* Global SOR variables */
int n, t;
double threshold;
double **val, **new;
double delta = 0.0;
double deltaNew = 0.0;
double globalDelta = 0.0;

/* Command line args: matrix size, threshold, number of threads */
int main(int argc, char * argv[]) {

  /* thread ids and attributes */
  pthread_t tid[MAXTHREADS];
  pthread_attr_t attr;
  long i, j;
  float myThreshold;
  double startTime, endTime, seqTime, parTime;
  
  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  
  /* initial mutex and condition variable */
  pthread_mutex_init(&update_lock, NULL);
  pthread_mutex_init(&barrier_lock, NULL);
  pthread_cond_init(&all_here, NULL);
  
  /* read command line arguments */
  if (argc != 4) {
    printf("usage: %s <matrix size> <threshold> <number of threads>\n",
	   argv[0]);
    exit(1);
  } // end if
  
  sscanf(argv[1], "%d", &n);
  sscanf(argv[2], "%f", &myThreshold);
  sscanf(argv[3], "%d", &t);
  threshold = (double) myThreshold;

  val = allocate2DArray(n+2, n+2);
  new = allocate2DArray(n+2, n+2);
  initializeData(val, n);
  initializeData(new, n);
  printf("InitializeData done\n");

  /* Time sequential SOR */
  GET_TIME(startTime);
  sequential2D_SOR();
  GET_TIME(endTime);
  printf("Sequential Time = %1.5f\n", endTime-startTime);
  printf("maximum difference:  %e\n\n", delta);

  /* Time parallel SOR using pthreads */
  initializeData(val, n);
  initializeData(new, n);
  GET_TIME(startTime);
  for(i=0; i<t; i++) {
    pthread_create(&tid[i], &attr, thread_main, (void *) i);
  } // end for
  
  for (i=0; i < t; i++) {
    pthread_join(tid[i], NULL);
  } // end for
  GET_TIME(endTime);
  printf("Parallel Time with %d threads = %1.5f\n", t, endTime-startTime);
  printf("maximum difference:  %e\n\n", delta);
  
} // end main


/***********************************************************************
 * Function sequential2D_SOR - performs a sequential 2D SOR calculation
 * using global variables:
 * val - 2D array initially with all 0.0 except 1.0s along "left" column
 *       "returns" the resulting value
 * new - 2D array used for storage during each iteration
 * n - array sizes are n x n 
 * threshold - max. value of all elements between two iterations
 * delta - returned max. value after last iteration (<= threshold)
 **********************************************************************/
void sequential2D_SOR() {
  double average, maxDelta, thisDelta;
  double ** temp;
  int i, j;
  
  do {
    maxDelta = 0.0;
    
    for (i = 1; i <= n; i++) {
      for (j = 1; j <= n; j++) {
	average = (val[i-1][j] + val[i][j+1] + val[i+1][j] + val[i][j-1])/4;
	thisDelta = fabs(average - val[i][j]);
	if (maxDelta < thisDelta) {
	  maxDelta = thisDelta;
	} // end if

	new[i][j] = average; // store into new array

      } // end for j
    } // end for i
    
    temp = new; /* prepare for next iteration */
    new = val;
    val = temp;
    
    // printf("maxDelta = %8.6f\n", maxDelta);
  } while (maxDelta > threshold);  // end do-while

  delta = maxDelta; // sets global delta

} // end sequential2D_SOR


void* thread_main(void * arg) {
  
  long id=(long) arg;
  delta = 1.0; //so conditional statement works

  double average, maxDelta, thisDelta;
  double ** temp;
  int i, j, blockSize, startRow, endRow;

  blockSize = n/t;
  startRow = (blockSize*id)+1;
  if (id < t-1) {
    endRow = (blockSize*(id+1));
  } else {
    endRow = n;
  }
    
  do {
    maxDelta = 0.0;
    
    for (i = startRow; i <= endRow; i++) {
      for (j = 1; j <= n; j++) {
	average = (val[i-1][j] + val[i][j+1] + val[i+1][j] + val[i][j-1])/4;
	thisDelta = fabs(average - val[i][j]);
	if (maxDelta < thisDelta) {
	  maxDelta = thisDelta;

	} // end if

	new[i][j] = average; // store into new array
	
      } // end for j
    } // end for i
  
    barrier(id);
 
    if (id == t-1) {
      temp = new; /* prepare for next iteration */
      new = val;
      val = temp;
    }

    // printf("maxDelta %lf < threshold %lf\n",maxDelta,threshold);
	
    if (maxDelta < threshold) {
      delta = maxDelta;
    }

    barrier(id);

    // printf("thread %d delta = %8.6f\n", id, delta);
  } while (delta >= threshold); //end do-while

  // printf("Hello from thread %d! I'm done!\n",id);
  
} // end thread_main



/*******************************************************************
 * Function allocate2DArray dynamically allocates a 2D array of
 * size rows x columns, and returns it.
 ********************************************************************/
double ** allocate2DArray(int rows, int columns) {
  double ** local2DArray;
  int r;

  local2DArray = (double **) malloc(sizeof(double *)*rows);

  for (r=0; r < rows; r++) {
    local2DArray[r] = (double *) malloc(sizeof(double)*columns);
  } // end for

  return local2DArray;
} // end allocate2DArray


/*******************************************************************
 * Function initializeData initializes 2D array for SOR with 0.0
 * everywhere, except 1.0s down column 0.
 ********************************************************************/
void initializeData(double ** array, int n) {
  int i, j;
  
  /* initialize to 0.0 except for 1.0s along the left boundary */
  for (i = 0; i < n+2; i++) {
    array[i][0] = 1.0;
  } // end for i

  for (i = 0; i < n+2; i++) {
    for (j = 1; j < n+2; j++) {
      array[i][j] = 0.0;
    } // end for j
  } // end for i

} // end initializeData


/*******************************************************************
 * Function print2DArray is passed the # rows, # columns, and the
 * array2D.  It prints the 2D array to the screen.
 ********************************************************************/
void print2DArray(int rows, int columns, double ** array2D) {
  int r, c;
  for(r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      printf("%10.5lf", array2D[r][c]);
    } // end for (c...
    printf("\n");
  } // end for(r...

} // end print2DArray



/*******************************************************************
 * Function equal2DArrays is passed the # rows, # columns, two 
 * array2Ds, and tolerance.  It returns TRUE if corresponding array
 * elements are equal within the specified tolerance; otherwise it
 * returns FALSE.
 ********************************************************************/
BOOL equal2DArrays(int rows, int columns, double ** array1, double ** array2,
		   double tolerance) {

  int r, c;

  for(r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      if (fabs(array1[r][c] - array2[r][c]) > tolerance) {
        return FALSE;
      } // end if
    } // end for (c...
  } // end for(r...
  return TRUE;

} // end equal2DArray



/*******************************************************************
 * Function barrier passed the thread id for debugging purposes.
 * Implements barrier synchronization using global variables:
 * count - # of thread that have arrived at the barrier
 * t - # of threads we are synchronizing
 * barrier_lock - the mutex ensuring mutual exclusion
 * all_here - the condition variable where threads wait for all to arrive
 ********************************************************************/
void barrier(long id) {
  pthread_mutex_lock(&barrier_lock);
  count++;
  // printf("count %d, id %d\n", count, id);
  if (count == t) {
    count = 0;
    pthread_cond_broadcast(&all_here);
  } else {
    while(pthread_cond_wait(&all_here, &barrier_lock) != 0);
  } // end if
  pthread_mutex_unlock(&barrier_lock);
} // end barrier

