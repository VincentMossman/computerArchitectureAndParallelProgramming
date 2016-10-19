/* Program to print a multiplication table using statically
   allocated 2D array and command-line arguments.
   Compile by:  gcc -o table printAddrs.c
   Run by:  ./addrs 5 5
*/
#include <stdio.h>
#include <stdlib.h>

const int SIZE = 20;

// function prototypes
void calculateProducts(int rows, int columns, int multiplicationTable[][SIZE]);
void printTableHeading(int value1, int value2);
void printRow(int row, int maxColumn, int rowProducts[]);

int main(int argc, char ** argv) {
  int multiplicationTable[SIZE][SIZE];  // statically allocated array
  int value1, value2, row;

  sscanf(argv[1], "%d", &value1);
  sscanf(argv[2], "%d", &value2);

  calculateProducts(value1, value2, multiplicationTable);

  printTableHeading(value1,  value2);

  for (row = 1; row <= value1; row++) {
    printRow(row, value2, multiplicationTable[row-1]);
  } // end for (row...

  // prints addresses in row pointers (%p in printf) array
  printf("\nPointers in the array of row pointers pointed at by multiplicationTable.\n");
  for (row = 0; row < value1; row++) {
    printf("pointer to row %d: %p\n", row, (void *) multiplicationTable[row]);
  } // end for

  return 0;
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
void printTableHeading(int value1, int value2) {
  int column;

  printf("\nThe multiplication table from 1 x 1 up to %d x %d\n\n",value1, value2);

  printf("  X  |");
  for (column = 1; column <= value2; column++) {
    printf("%5d", column);
  } // end for (column...
  printf("\n");

  printf("-----|");
  for (column = 1; column <= value2; column++) {
    printf("-----");
  } // end for (column...
  printf("\n");

} // end printTableHeading


/*******************************************************************
 * Function printRow is passed a row value, the value2 (i.e., max. column
 * value), and corresponding rowProducts array which is printed
 ********************************************************************/
void printRow(int row, int maxColumn, int rowProducts[]) {
  int column;
  
  printf("%-5d|", row);
  for (column = 0; column < maxColumn; column++) {
    printf("%5d", rowProducts[column]);
  } // end for (column...
  printf("\n");
  printf("addrs: ");
  for (column = 0; column < maxColumn; column++) {
    printf("%p ", (void *) &(rowProducts[column]));
  } // end for (column...
  printf("\n\n");
} // end printRow
