/* Program to generate two square 2D arrays of random doubles and
   time their multiplication.
   Compile by:  gcc -O5 -o mmult mmultSeqOptions.c -lm
   Run by:  ./mmult 1000
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  // use the time to seed the random # generator
#include <math.h>  // needed for fabs function

#define TRUE 1
#define FALSE 0
#define BOOL int

// function prototypes
double ** allocate2DArray(int rows, int columns);
void print2DArray(int rows, int columns, double ** array2D);
void generateRandom2DArray(int rows, int columns, 
			   double min, double max, double ** random2DArray);
BOOL equal2DArrays(int rows, int columns, double ** array1, double ** array2,
		   double tolerance);
void matrixMultiplication(int rows1, int columns1, double ** array1, 
			  int rows2, int columns2, double ** array2,
			  double ** product);
void matrixMultiplicationAlt(int rows1, int columns1, double ** array1, 
			     int rows2, int columns2, double ** array2,
			     double ** product);

int main(int argc, char ** argv) {
  double ** A;
  double ** B;
  double ** C;
  double ** C_alt;
  int rows, columns;
  long startTime, endTime,seqTime;
  
  if (argc !=2) {
    printf("Usage: %s <# integer matrix size>\n", argv[0]);
    exit(-1);     
  } // end if

  sscanf(argv[1], "%d", &rows);
  columns = rows;

  // seed the random number generator
  srand( time(NULL) );

  A = allocate2DArray(rows, columns);
  B = allocate2DArray(rows, columns);
  C = allocate2DArray(rows, columns);
  C_alt = allocate2DArray(rows, columns);
  generateRandom2DArray(rows, columns, -1.0, +1.0, A);
  generateRandom2DArray(rows, columns, -1.0, +1.0, B);

  printf("after initializing matrices\n");

  time(&startTime);

  matrixMultiplication(rows, columns, A, rows, columns, B, C);

  time(&endTime);
  seqTime = endTime-startTime;
  printf("Matrix Multiplication time = %ld\n",seqTime);

  time(&startTime);

  matrixMultiplicationAlt(rows, columns, A, rows, columns, B, C_alt);

  time(&endTime);
  seqTime = endTime-startTime;
  printf("Matrix Multiplication Alt. time = %ld\n",seqTime);

  if (equal2DArrays(rows, columns, C, C_alt, 0.0)) {
    printf("Arrays match with tolerance of %.10f\n", 0.0);
  } else {
    printf("Arrays DON'T match with tolerance of %.10f\n", 0.0);
  } // end if

  // if small enough, print to screen
  if (rows < 10 && columns < 10) {
    printf("C 2D array of doubles:\n");
    print2DArray(rows, columns, C);
    printf("\nC_alt 2D array of doubles:\n");
    print2DArray(rows, columns, C);
  } // end if

  return 0;

} // end main


/*******************************************************************
 * Function matrixMultiplication passed two matrices and returns
 * their product.
 ********************************************************************/
void matrixMultiplication(int rows1, int columns1, double ** array1, 
			  int rows2, int columns2, double ** array2,
			  double ** product) {
  int i, j, k;

  if (columns1 != rows2) {
    printf("Matrices cannot be multiplied -- incompatible dimensions!\n");
    exit(-1);
  } // end if

  for (i=0; i < rows1; i++) {
    for (j=0; j < columns2; j++) {
      product[i][j] = 0.0;
      for (k=0; k < columns1; k++) {
        product[i][j] += array1[i][k]*array2[k][j];
      } /* end for (k */
    } /* end for (j */
  } /* end for (i */

} // end matrixMultiplication



/*******************************************************************
 * Function matrixMultiplicationAlt passed two matrices and returns
 * their product.
 ********************************************************************/
void matrixMultiplicationAlt(int rows1, int columns1, double ** array1, 
			     int rows2, int columns2, double ** array2,
			     double ** product) {
  int i, j, k;
  double ** array2_transpose;
  
  if (columns1 != rows2) {
    printf("Matrices cannot be multiplied -- incompatible dimensions!\n");
    exit(-1);
  } // end if

  // Transposes array2
  array2_transpose = allocate2DArray(columns2, rows2);
  for (i=0; i < rows2; i++) {
    for (j=0; j < columns2; j++) {
      array2_transpose[j][i] = array2[i][j];
    } /* end for (j */
  } /* end for (i */

  // Matrix Multiplication uses array1 and array2_transpose
  for (i=0; i < rows1; i++) {
    for (j=0; j < columns2; j++) {
      product[i][j] = 0.0;
      for (k=0; k < columns1; k++) {
        product[i][j] += array1[i][k]*array2_transpose[j][k];
      } /* end for (k */
    } /* end for (j */
  } /* end for (i */

} // end matrixMultiplicationAlt



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
 * Function generateRandom2DArray is passed the # rows, the # columns,
 * min. value, max. value, and returns random2DArray containing 
 * randomly generated doubles.
 ********************************************************************/
void generateRandom2DArray(int rows, int columns, 
			   double min, double max, double ** random2DArray) {
  int r, c;
  double range, div;

  for (r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      range = max - min;
      div = RAND_MAX / range;
      random2DArray[r][c] = min + (rand() / div);
    } // end for (c...
  } // end for (r...
} // end generateRandom2DArray


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





