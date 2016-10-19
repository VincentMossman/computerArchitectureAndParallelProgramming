/* Edited by: Vincent T. Mossman
   Author:    Mark Fienup
   Program to generate two square 2D arrays of random doubles using
   threads and time their addition sequentially and using pthreads.
   The resulting sum matrix is assigned to threads by blocks of
   whole rows.  
   Compile by:  gcc -O5 -o maddC maddC.c -lm -lpthread
   Run by:  ./mmultC 1000 2000 8
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  // use the time to seed the random # generator
#include <math.h>  // needed for fabs function
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define BOOL int

typedef struct {
  int threadId;
  int start_row;
  int end_row;
  int start_col;
  int end_col;
  double ** array;
  double ** array2;
  double min;
  double max;
} BLOCK;


// function prototypes
double ** allocate2DArray(int rows, int columns);
void print2DArray(int rows, int columns, double ** array2D);
BOOL equal2DArrays(int rows, int columns, double ** array1, double ** array2,
		   double tolerance);
void matrixAddition(int rows1, int columns1, double ** array1, double ** array2,
                    double ** arraySum);
void * threadGenerate2DBlockThenAdd(void * block);
void barrier(long i);


// Global variables
double ** A;
double ** B;
double ** Sum;
int numberOfThreads;
int rows, columns;
pthread_mutex_t update_lock;
pthread_mutex_t barrier_lock;   /* mutex for the barrier */
pthread_cond_t all_here;        /* condition variable for barrier */
int count=0;                    /* counter for barrier */


int main(int argc, char ** argv) {
  double ** C_seq;
  long i, initializationTime, startTime, endTime, seqTime, parallelTime;
  pthread_t * threadHandles;
  int errorCode;
  double ** Sum_seq;
  double tolerance;
  BLOCK * blocksOfWork;
  BLOCK * block;
  
  
  if (argc !=4) {
    printf("Usage: %s <# rows> <# columns> <# threads>\n", argv[0]);
    exit(-1);     
  } // end if

  sscanf(argv[1], "%d", &rows);
  sscanf(argv[2], "%d", &columns);
  sscanf(argv[3], "%d", &numberOfThreads);

  // seed the random number generator
  srand( 5 );

  A = allocate2DArray(rows, columns);
  B = allocate2DArray(rows, columns);
  Sum = allocate2DArray(rows, columns);
  Sum_seq = allocate2DArray(rows, columns);

  time(&startTime);
   // Generate arrays for threads handles
  threadHandles = (pthread_t *) malloc(numberOfThreads*sizeof(pthread_t));
  block = (BLOCK *) malloc(numberOfThreads*sizeof(BLOCK));
  
    // create threads to randomly generate A and B
  for (i=0; i < numberOfThreads; i++) {
    block[i].threadId = i;
    block[i].start_row = i*rows/numberOfThreads;
    block[i].end_row = (i+1)*rows/numberOfThreads - 1;
    block[i].start_col = 0;
    block[i].end_col = columns - 1;
    block[i].array = A;
    block[i].array2 = B;
    block[i].min = 0.0;
    block[i].max = +5.0;
    pthread_create(&threadHandles[i], NULL, threadGenerate2DBlockThenAdd, &block[i]);
  } // end for
  
  for (i=0; i < numberOfThreads; i++) {
    pthread_join(threadHandles[i], (void **) NULL);
  } // end for

  time(&endTime);
  initializationTime = endTime-startTime;
  printf("Matrix Initialization and Parallel time = %ld\n", initializationTime);

 
  time(&startTime);

  matrixAddition(rows, columns, A, B, Sum_seq);

  time(&endTime);
  seqTime = endTime-startTime;
  printf("Seq. Matrix Addition time = %ld\n", seqTime);

  tolerance = 0.0000001;
  if (equal2DArrays(rows, columns, Sum, Sum_seq, tolerance)) {
    printf("Arrays match with tolerance of %.10f\n", tolerance);
  } else {
    printf("Arrays DON'T match with tolerance of %.10f\n", tolerance);
  } // end if

  // if small enough, print to screen
  if (rows < 10 && columns < 10) {
    printf("Matrix Sum sequential:\n");
    print2DArray(rows, columns, Sum_seq);
    printf("\nMatrix Sum parallel:\n");
    print2DArray(rows, columns, Sum);
  } // end if

  return 0;

} // end main

/*******************************************************************
 * Function threadGenerate2DBlock - each thread is passed a BLOCK structure
 * define the block of an array to randomly generate.
 * NOTE:  rand_r is used to generate random #'s because it is "thread safe"
 ********************************************************************/
void * threadGenerate2DBlockThenAdd(void * arg) {
  BLOCK * block = (BLOCK *) arg;
  int threadId = block->threadId;
  int startRow = block->start_row;
  int endRow = block->end_row;
  int startCol = block->start_col;
  int endCol = block->end_col;
  double ** array = block->array;
  double ** array2 = block->array2;
  double min = block->min;
  double max = block->max;
  int r, c, blockSize;
  double range, div;
  int seed = (threadId+1)*1000;

  for (r = startRow; r <= endRow; r++) {
    for (c = startCol; c <= endCol; c++) {
      range = max - min;
      div = RAND_MAX / range;
      array[r][c] = min + (rand_r(&seed) / div);
    } // end for (c...
  } // end for (r...
  // thread changes it's role to start computing matrix addition on
  // a block of rows of Sum matrix
  
  for (r = startRow; r <= endRow; r++) {
    for (c = startCol; c <= endCol; c++) {
      range = max - min;
      div = RAND_MAX / range;
      array2[r][c] = min + (rand_r(&seed) / div);
    }
  }

  // thread's block Matrix Multiplication uses A and B_transpose
  // Matrix Multiplication uses array1 and array2_transpose
  for (r = startRow; r <= endRow; r++) {
    for (c = startCol; c <= endCol; c++) {
        Sum[r][c] += A[r][c]+B[r][c];
    } /* end for (j */
  } /* end for (i */

} // threadGenerate2DBlockThenAdd



/*******************************************************************
 * Function barrier passed the thread id for debugging purposes.
 * Implements barrier synchronization using global variables:
 * count - # of thread that have arrived at the barrier
 * numberOfThreads - # of threads we are synchronizing
 * barrier_lock - the mutex ensuring mutual exclusion
 * all_here - the condition variable where threads wait for all to arrive
 ********************************************************************/
void barrier(long id) {
  pthread_mutex_lock(&barrier_lock);
  count++;
  //printf("count %d, id %d\n", count, id);
  if (count == numberOfThreads) {
    count = 0;
    pthread_cond_broadcast(&all_here);
  } else {
    while(pthread_cond_wait(&all_here, &barrier_lock) != 0);
  } // end if
  pthread_mutex_unlock(&barrier_lock);
} // end barrier



/*******************************************************************
 * Function matrixAddition - passed the dimenstions of two 2D arrays,
 * two 2D arrays, and returns the resulting 2D sum array.
 ********************************************************************/
void matrixAddition(int rows, int columns, double ** array1, double ** array2,
                    double ** arraySum) {
  int r, c;

  for (r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      arraySum[r][c] = array1[r][c] + array2[r][c];
    } // end for (c...
  } // end for (r...

} // end matrixAddition



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





