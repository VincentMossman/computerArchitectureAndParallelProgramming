/*  File: sum1DArray.c
 *  Compile as:  mpicc -o sum1DArrayA -O3 sum1DArrayA.c
 *  Run by:      qsub qsub.sum1DArrayA 
 *  Description:  An MPI solution to sum a 1D array. */

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
  double seqSum, parallelSum, localSum, recvSum;
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
    
    /* Send a message with part of array to each MPI process*/
    for (p=0; p<numProcs-1; p++) {
       MPI_Send( myArray+length_per_process*p, length_per_process, MPI_FLOAT, p+1, tag, MPI_COMM_WORLD );
   } // end for p

    /* Do the actual work - sum "right" end of array*/
    localSum = 0.0;
    for (i= length_per_process*(numProcs-1);i < length; i++) {
      localSum += myArray[i];
    } // end for i

    parallelSum = localSum;
    /* collect up localSum's from all other processes */
    for (p=0; p < numProcs-1; p++) {
      MPI_Recv( &recvSum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status );
      parallelSum += recvSum;
    } // end for
    
  } else {  // code for MPI processes that are not the root process
    myArray = (float *) malloc(length_per_process*sizeof(float));
    MPI_Recv( myArray, length_per_process, MPI_FLOAT, RootProcess, tag, MPI_COMM_WORLD, &status );
    
    /* Do the actual work */
    localSum = 0.0;
    for (i=0; i < length_per_process; i++) {
      localSum += myArray[i];
    } // end for i

    MPI_Send( &localSum, 1, MPI_DOUBLE, RootProcess, tag, MPI_COMM_WORLD );
    

  } // end if

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
    
  } // end if
  free(myArray);
  
  MPI_Finalize();
  return 0;
} /* end main */ 

