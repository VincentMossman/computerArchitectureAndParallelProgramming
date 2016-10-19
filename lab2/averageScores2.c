/* Program to demonstrate using dynamic allocation
   of arrays using malloc.
   Compile by:  gcc -o average averageScores2.c
   Run by:  ./average
*/
#include <stdio.h>
#include <stdlib.h>

// function prototypes
void getScores(int *, double **);
double calculateAverage(int, double []);

int main() {
  double * scores;
  int numberOfScores;
  double average;

  getScores(&numberOfScores, &scores);
  printf("numberOfScores = %d\n", numberOfScores);  
  average = calculateAverage(numberOfScores, scores);

  printf("The average of the %d scores is %.1f.\n", numberOfScores, average);

  free(scores);  // deallocates array

} // end main


/*******************************************************************
 * Function getScores asks the user to enter scores using any negative
 * as a sentinel value.  It returns the scores and the number of scores.
 ********************************************************************/
void getScores(int * count, double ** scores) {
  double score;
  double * localScoresPtr;
  int i;

  printf("Enter the # of scores you will be entering: ");
  scanf("%d", count);
  localScoresPtr = (double *) malloc(sizeof(double)*(*count));

  for (i = 0; i < *count; i++) {
    printf("Enter a score: ");
    scanf("%lf", &score);
    localScoresPtr[i] = score;
  } // end for

  *scores = localScoresPtr;

} // end getScores


double calculateAverage(int numberOfScores, double scores[]) {
  double total;
  int i;

  total = 0.0;
  for(i = 0; i < numberOfScores; i++) {
    total = total + scores[i];
  } // end for

  return total / numberOfScores;
} // end calculateAverage
