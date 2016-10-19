/*  Programmer:  Mark Fienup
    File:        seq2DSOR.c
    Compiled by: gcc -o seq2DSOR seq2DSOR.c -lpthread -lm
    Run by:      seq2DSOR 5 0.01
    Run by:      seq2DSOR <interior matrix size> <threshold>
    Description:  Sequential 2D over-relaxation program.
*/
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include "timer.h"

/* Prototypes */
void InitializeData();
void perform2D_SOR();
void printVal();

int n, t;
double threshold, delta;
double **val, **new;


/* Command line args: matrix size, threshold */
int main(int argc, char * argv[]) {
  long i, j;
  float myThreshold;
  double startTime, endTime;
  
  /* read command line arguments */
  if (argc != 3) {
    printf("usage: %s <interior matrix size> <threshold>\n",
	   argv[0]);
    exit(1);
  } // end if
    
  sscanf(argv[1], "%d", &n);
  sscanf(argv[2], "%f", &myThreshold);
  threshold = (double) myThreshold;
  
  InitializeData();
  printf("Initialize Data done\n");

  GET_TIME(startTime)
  /* Use global variables like the pthread verision */
  perform2D_SOR();
  
  GET_TIME(endTime);
  printf("Time = %1.5f\n", endTime-startTime);

  printf("maximum difference:  %8.6f\n", delta);
    
} // end main


void perform2D_SOR() {
  
  double average, maxDelta;
  double thisDelta;
  double **temp;
  int i, j;
  
  do {
    maxDelta = 0.0;
    
    /* printVal(aaa); */

    for (i = 1; i <= n; i++) {
      for (j = 1; j <= n; j++) {
	average = (val[i-1][j] + val[i][j+1] +
		   val[i+1][j] + val[i][j-1])/4;
	thisDelta = fabs(average - val[i][j]);
	if (maxDelta < thisDelta) {
	  maxDelta = thisDelta;
	} // end if

	// store into new array
	new[i][j] = average;
      } // end for j
    } // end for i
    
    temp = new;		/* prepare for next iteration */
    new = val;
    val = temp;
    
    /* printf("maxDelta = %8.6f\n", maxDelta); */
  } while (maxDelta > threshold);  // end do-while

  delta = maxDelta; // sets global 

} // end perform2D_SOR

void printVal() {
  int i,j;
  printf("\n\n*2222**********************************************\n");
  for (i = 0; i < n+2; i++) {
    for (j = 0; j < n+2; j++) {
      printf("%7.3f ", val[i][j]);
    } // end for j
    printf("\n");
  } // end for i
} // end printVal

void InitializeData() {
	int i, j;

	new = (double **) malloc((n+2)*sizeof(double *));
	val = (double **) malloc((n+2)*sizeof(double *));

	for (i = 0; i < n+2; i++) {
		new[i] = (double *) malloc((n+2)*sizeof(double));
		val[i] = (double *) malloc((n+2)*sizeof(double));
	} // end for i

	/* initialize to 0.0 except to 1.0 along the left boundary */
	for (i = 0; i < n+2; i++) {
		val[i][0] = 1.0;
		new[i][0] = 1.0;
	} // end for i
	for (i = 0; i < n+2; i++) {
		for (j = 1; j < n+2; j++) {
			val[i][j] = 0.0;
			new[i][j] = 0.0;
		} // end for j
	} // end for i
} // end InitializeData
