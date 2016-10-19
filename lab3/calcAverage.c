/* File:  calcAverage.c
   Description:  Averages numbers typed on the
   command-line.
   Compile by: gcc -o average calcAverage.c
   Run by: ./average 10 20 30 40
   Output:  The average is 25.0.
*/

#include <stdlib.h>
#include <stdio.h>

// prototype
double calculateAverage(int numberOfScores, double scores[]);

int main(int argc, char ** argv) {
  double * values;
  int index, numberOfValues;
  double average;

  if (argc == 1) {
    printf("Usage:  %s <followed by a list of numbers to average separated by spaces>\n"
	   , argv[0]);
    exit(-1);
  }

  numberOfValues = argc - 1;

  // allocate array for numbers on command-line
  values = (double *) malloc(sizeof(double)*numberOfValues);

  // sscanf (string formatted scan) - scans each 
  // command-line argument string and converts it to a double
  for (index = 0; index < numberOfValues; index++) {
    // uses indexing with [] to address/access array elements
    sscanf(argv[index+1],"%lf",&values[index]);

    // uses pointer arithmetic to address/access array elements
    //sscanf(*(argv+index+1),"%lf",values+index);

    // uses a combination of indexing and pointer arithmetic
    //sscanf(argv[index+1],"%lf",values+index);
  } // end for

  average = calculateAverage(numberOfValues, values);

  printf("The average is %.1f.\n", average);

  free(values);   // deallocates the array

  return 0;

} // end main


/************************************************************
 * Function that's passed the number of scores and an array 
 * of that many doubles.  It returns the average of the 
 * array values.
 ************************************************************/
double calculateAverage(int numberOfScores, double scores[]) {
  double total;
  int i;

  total = 0.0;
  for(i = 0; i < numberOfScores; i++) {
    total = total + *(scores+i);
  } // end for

  if (numberOfScores == 0) {
    exit(-1);
  } else {
    return total / numberOfScores;
  } // end if
} // end calculateAverage
