/* Program to demonstrate using arrays including declaring arrays,
   passing arrays as parameters to functions, and dynamic allocation
   of arrays using malloc.
   Compile by:  gcc -o average averageScores.c
   Run by:  ./average
*/
#include <stdio.h>
#include <stdlib.h>

// function prototypes
void getScores(int *, double []);
double calculateAverage(int, double []);

int main() {
  double scores[100];
  int numberOfScores;
  double average;

  getScores(&numberOfScores, scores);
  printf("numberOfScores = %d\n", numberOfScores);  
  average = calculateAverage(numberOfScores, scores);

  printf("The average of the %d scores is %.1f.\n", numberOfScores, average);

} // end main


/*******************************************************************
 * Function getScores asks the user to enter scores using any negative
 * as a sentinel value.  It returns the scores and the number of scores.
 ********************************************************************/
void getScores(int * count, double scores[]) {
  double score;
  
  printf("Enter scores one at a time (enter -1 to quit).\n");
  *count = 0;
  while (1) {  // infinite loop any nonzero integer is True
    printf("Enter a score (or -1 when done): ");
    scanf("%lf", &score);
    if (score < 0.0) {
      break;
    } // end if
    scores[*count] = score;
    (*count)++;
  } // end while

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
