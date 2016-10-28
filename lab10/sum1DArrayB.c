/*  File: sum1DArray.c
 *  Compile as:  mpicc -o sum1DArrayB -O3 sum1DArrayB.c
 *  Description:  An MPI solution to sum a 1D array. 
 *  Uses group communications:
 *      - MPI_Scatterv to send blocks of 1D
 *      - MPI_Reduce
 *  array to each
*/

#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <mpi.h>
#include "timer.h"

#define RootProcess 0

const int tag = 1;

int main(int argc, char* argv[]) {
  int myID, value, numProcs, i, p;
  float * myArray;
  float * localArray;
  double seqSum, parallelSum, localSum, recvSum;
  int * counts;
  int * displacements;
  int length;
  int length_per_process;
  double clockStart, clockEnd;
  
  GET_TIME(clockStart);

  MPI_Status status;
  
  MPI_Init(&argc, &argv);  /* Initialize MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs); /* Get rank */
  MPI_Comm_rank(MPI_COMM_WORLD, &myID); /* Get rank */

  // all processes have access to argc and argv
  sscanf(argv[1],"%d",&length);
  length_per_process = length/numProcs;
  
  if ( myID == RootProcess ) { 
    if (argc != 2) {
      printf("Usage: %s <# of floats to sum>\n", argv[0]);
      return(0);
    };
    
    // Generate data array   length = SIZE;
    printf("length = %d\n", length);
    myArray=(float *) malloc(length*sizeof(float));
    srand(5);
    for (i=0; i < length; i++) {
      myArray[i] = rand() / (float) RAND_MAX;
    } // end for i
    
  }  // end if (myID == RootProcess

  // code for all MPI processes 

  localArray = (float *) malloc(length_per_process*sizeof(float));
  printf("myID %d\n", myID);
  // distributes same sized chucks of data to all processes
  MPI_Scatter( myArray, length_per_process, MPI_FLOAT, localArray,
	       length_per_process, MPI_FLOAT, RootProcess,  MPI_COMM_WORLD );
  
  /* Do the actual work */
  localSum = 0.0;
  for (i=0; i < length_per_process; i++) {
    localSum += localArray[i];
  } // end for i

  MPI_Reduce(&localSum, &parallelSum, 1, MPI_DOUBLE, MPI_SUM, RootProcess, MPI_COMM_WORLD);
  
  GET_TIME(clockEnd);
  
  if (myID == RootProcess) {
    printf( "Time to sum %d floats with MPI in parallel  %3.5f seconds\n", length,
	    (clockEnd - clockStart));
    GET_TIME(clockStart);
    seqSum = 0.0;
    for (i=0; i < length; i++) {
      seqSum += myArray[i];
    } // end for i
    GET_TIME(clockEnd);
    printf( "Time to sum %d floats sequentially  %3.5f seconds\n", length,
            (clockEnd - clockStart));

    printf("The parallel sum:   %f\n", parallelSum);
    printf("The sequential sum: %f\n", seqSum);
    free(myArray);
    
  } // end if
  free(localArray);
  
  MPI_Finalize();
  return 0;
} /* end main */ 

