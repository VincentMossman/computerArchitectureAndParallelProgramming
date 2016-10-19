/*  Programmer:  Mark Fienup
    File:        sum1DArray_pthreads_NoSync.c
    Compile As:  gcc -o sumArray -O0 sumNoSync.c -lpthread
    Compile As:  gcc -o sumArray -O3 sumNoSync.c -lpthread
    Run as:	 ./sumArray <# of floats to sum> <# of pthreads>
    Description:  A parallel C solution to sum an array of floats
                  using pthreads, BUT IT IS INCORRECT.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

// Prototypes
void  * threadPartialSum(void * args);

// GLOBAL variables
double globalSum;
int numberOfThreads;
long length;
float * myArray;


int main(int argc, char* argv[]) {
  long i;
  clock_t clockStart, clockEnd;
  pthread_t * threadHandles;
  pthread_attr_t * threadAttrs;
  int errorCode;
  double seqSum;

  if (argc != 3) {
    printf("Usage: %s <# of floats to sum> <# of threads>\n", argv[0]);
    return(0);
  };

  sscanf(argv[1],"%d",&length);
  sscanf(argv[2],"%d",&numberOfThreads);

  // Generate arrays for threads handles
  threadHandles = (pthread_t *) malloc(numberOfThreads*sizeof(pthread_t));
  threadAttrs = (pthread_attr_t *) malloc(numberOfThreads*sizeof(pthread_attr_t));

  // Generate data array
  myArray=(float *) malloc(length*sizeof(float));
  srand(5);
  seqSum = 0.0;
  for (i=0; i < length; i++) {
    myArray[i] = rand() / (float) RAND_MAX;
    seqSum += myArray[i];
  } // end for i

  clockStart = clock();

  for (i=0; i < numberOfThreads; i++) {
    pthread_attr_init(&threadAttrs[i]);
    pthread_attr_setscope(&threadAttrs[i], PTHREAD_SCOPE_SYSTEM);
    if (errorCode = pthread_create(&threadHandles[i],  &threadAttrs[i], 
				   threadPartialSum, (void *) i) != 0) {
      printf("pthread %d failed to be created with error code %d\n", i, errorCode);
    } // end if
  } // end for
    
  for (i=0; i < numberOfThreads; i++) {
    if (errorCode = pthread_join(threadHandles[i], (void **) NULL) != 0) {
      printf("pthread %d failed to be joined with error code %d\n", i, errorCode);
    } // end if
  } // end for

  clockEnd = clock();

  printf( "Time to sum %ld floats using %d threads  %3.5f seconds\n", length, 
	  numberOfThreads, (clockEnd - clockStart) / (float) CLOCKS_PER_SEC);

  /* printf("clockStart = %d   clockEnd = %d  CLOCKS_PER_SEC = %d\n", */
  /* 	 clockStart, clockEnd, CLOCKS_PER_SEC); */

  printf("Thread's Sum is %lf and seq. sum %lf\n",globalSum, seqSum);

  free(myArray);

  return 0;
} /* end main */

void * threadPartialSum(void * rank) {
  
  long myRank = (long) rank;
  long i, blockSize;
  long firstIndex, lastIndex;
  double localSum;

  blockSize = length / numberOfThreads;
  firstIndex = blockSize * myRank;
  if (myRank == numberOfThreads-1) { // last thread gets the rest
    lastIndex = length-1;
  } else {
    lastIndex = blockSize * (myRank+1) - 1;
  } // end if


  localSum = 0.0;
  for (i=firstIndex; i <= lastIndex; i++) {
    localSum += myArray[i];
    globalSum += myArray[i];
  } /* end for (i */
  printf("ThreadId %ld generated partial sum %f\n", myRank, localSum);
  return NULL;
} // end threadPartialSum
