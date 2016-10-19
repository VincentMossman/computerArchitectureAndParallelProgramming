/* Program to display rectangle information
   Compile with math library: gcc -o rectangle displayRectangleInfo.c -lm
   Run by: rectangle
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//function prototypes
void getDimensions(double *, double *);
void calculateAreaAndCircumference(double, double, double *, double *);
void displayRectangleInformation(double, double, double, double);

int main() {

  double length, width, area, circumference;

  getDimensions(&length, &width);

  calculateAreaAndCircumference(length, width, &area, &circumference);

  displayRectangleInformation(length, width, area, circumference);

} // end main

/*******************************************************************
 * Function getDimensions asks the user to enter the length and     *
 * width of a rectangle and returns the length and width.           *
/********************************************************************/
void getDimensions(double * pLength, double * pWidth) {

  printf("Enter the length of a rectangle: ");
  scanf("%lf", pLength);
  
  printf("Enter the width of a rectangle: ");
  scanf("%lf", pWidth);

} // end GetDimensions

/*******************************************************************
 * Function calculateAreaAndCircumference is passed the length and  *
 * width of a rectangle and it returns the rectangle's area and     *
 * circumference (perimeter).                                       *
/********************************************************************/
void calculateAreaAndCircumference(double length, double width, double * pArea, double * pCircumference) {

  *pArea = length * width;
  *pCircumference = 2 * (length + width);

} // end calculateAreaAndCircumference

/*******************************************************************
 * Function displayRectangleInformation is passed the length, width,*
 * area, and circumference of a rectangle and displays this         *
 * information to the user. Nothing is returned.
/********************************************************************/
void displayRectangleInformation(double length, double width, double area, double circumference) {

  printf("A rectangle with a length of %3.2f and a width of %3.2f has an area of %3.2f\n", length, width, area);
  printf("and a circumference of %3.2f.\n", circumference);

} // end displayRectangleInformation
