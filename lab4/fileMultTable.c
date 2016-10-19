/* Program to print a multiplication table to a text-file.
   Compile by:  gcc -o table fileMultTable.c
   Run by:  ./table 10 10
*/
#include <stdio.h>
#include <stdlib.h>

const int SIZE = 20;

// function prototypes
void calculateProducts(int rows, int columns, int multiplicationTable[][SIZE]);
void printTableHeading(FILE * outputFilePtr, int value1, int value2);
void printRow(FILE * outputFilePtr, int row, int maxColumn, int rowProducts[]);

int main(int argc, char ** argv) {
  FILE *outputFilePtr;
  int multiplicationTable[SIZE][SIZE];  // statically allocated array
  int value1, value2, row, column;

  if (argc !=4) {
    printf("Usage: %s <integer> <integer> <filename.txt>\n", argv[0]);
    exit(-1);     
  } // end if

  if ((outputFilePtr = fopen(argv[3], "w")) == NULL) {
    printf("%s cannot be opened for writing\n", argv[3]);
    exit(-1);
  } // end if

  sscanf(argv[1], "%d", &value1);
  sscanf(argv[2], "%d", &value2);

  calculateProducts(value1, value2, multiplicationTable);

  printTableHeading(outputFilePtr, value1,  value2);

  for (row = 1; row <= value1; row++) {
    printRow(outputFilePtr, row, value2, multiplicationTable[row-1]);
  } // end for (row...

  fclose(outputFilePtr);

} // end main


/*******************************************************************
 * Function calculateProducts is passed the # rows, the # columns,
 * and returns the multiplcationTable from (1 x 1) to (rows x columns)
 ********************************************************************/
void calculateProducts(int rows, int columns, int multiplicationTable[][SIZE]) {
  int r, c;

  for (r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      multiplicationTable[r][c] = (r+1) * (c+1);
    } // end for (c...
  } // end for (r...
} // end calculateProducts


/*******************************************************************
 * Function printTableHeading is passed the value1 and value2 values 
 * and prints the multiplication table header.
 ********************************************************************/
void printTableHeading(FILE * outputFilePtr, int value1, int value2) {
  int column;

  fprintf(outputFilePtr, "\nThe multiplication table from 1 x 1 up to %d x %d\n\n",
	  value1, value2);

  fprintf(outputFilePtr, "  X  |");
  for (column = 1; column <= value2; column++) {
    fprintf(outputFilePtr, "%5d", column);
  } // end for (column...
  fprintf(outputFilePtr, "\n");

  fprintf(outputFilePtr, "-----|");
  for (column = 1; column <= value2; column++) {
    fprintf(outputFilePtr, "-----");
  } // end for (column...
  fprintf(outputFilePtr, "\n");

} // end printTableHeading


/*******************************************************************
 * Function printRow is passed a row value, the value2 (i.e., max. column
 * value), and corresponding rowProducts array which is printed
 ********************************************************************/
void printRow(FILE * outputFilePtr, int row, int maxColumn, int rowProducts[]) {
  int column;
  
  fprintf(outputFilePtr, "%-5d|", row);
  for (column = 0; column < maxColumn; column++) {
    fprintf(outputFilePtr, "%5d", rowProducts[column]);
  } // end for (column...
  fprintf(outputFilePtr, "\n");
} // end printRow
