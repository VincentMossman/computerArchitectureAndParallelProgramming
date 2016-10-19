/* Program to print a multiplication table to allow you practice using 
   dynamic allocation of arrays using malloc.
   Compile by:  gcc -o multTable1D multTable2Din1D.c
   Run by:  ./multTable1D 4 4
*/
#include <stdio.h>
#include <stdlib.h>

// function prototypes
int * allocate2DArrayIn1D(int rows, int columns);
void calculateProducts(int rows, int columns, int * multiplicationTable);
void printTableHeading(int value1, int value2);
void printRow(int row, int maxColumn, int * rowProducts);

int main(int argc, char ** argv) {
  int * multiplicationTable;
  int value1, value2, row, column;

  sscanf(argv[1], "%d", &value1);
  sscanf(argv[2], "%d", &value2);

  multiplicationTable = allocate2DArrayIn1D(value1, value2);

  calculateProducts(value1, value2, multiplicationTable);

  printTableHeading(value1,  value2);

  for (row = 1; row <= value1; row++) {
    printRow(row, value2, multiplicationTable+(row-1)*value2);
  } // end for (row...
} // end main



/*******************************************************************
 * Function allocate2DArray dynamically allocates a 2D array of 
 * size rows x columns, and returns it.
 ********************************************************************/
int * allocate2DArrayIn1D(int rows, int columns) {
  int * local2DArray;

  local2DArray = (int *) malloc(sizeof(int *)*rows*columns);
  return local2DArray;
} // end allocate2DArray



/*******************************************************************
 * Function calculateProducts is passed the # rows, the # columns,
 * and returns the multiplcationTable from (1 x 1) to (rows x columns)
 ********************************************************************/
void calculateProducts(int rows, int columns, int * multiplicationTable) {
  int r, c;
  int * nextElementPtr;

  nextElementPtr = multiplicationTable;
  for (r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      // *(multiplicationTable+r*columns+c) = (r+1) * (c+1);
      *nextElementPtr++ = (r+1) * (c+1);
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
void printRow(int row, int maxColumn, int * rowProducts) {
  int * nextElementPtr;
  int column;
  
  nextElementPtr = rowProducts;
  printf("%-5d|", row);
  for (column = 0; column < maxColumn; column++) {
    printf("%5d", *nextElementPtr++);
  } // end for (column...
  printf("\n");
} // end printRow
