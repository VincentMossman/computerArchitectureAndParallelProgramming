/* Program to demonstrate functions and parameter passing.
   Compile with math library:  gcc -o circle displayCircleInfo.c -lm
   Run by:  circle 
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// function prototypes
double getRadius();
void calculateAreaAndCircumference(double, double *, double *);
void displayCircleInformation(double, double, double);

// global constants
const double PI = 3.14159;

int main() {
  double radius, area, circumference;
 
  radius = getRadius();
  
  calculateAreaAndCircumference(radius, &area, &circumference);

  displayCircleInformation(radius, area, circumference);

} // end main


/*******************************************************************
 * Function getRadius asks the user to enter the radius of a circle *
 * and returns the radius.                                          *
 ********************************************************************/
double getRadius() {
  double radius;
  
  printf("Enter the radius of a circle: ");
  scanf("%lf", &radius); // NOTE %lf for double, but %f for float
  return radius;
} // end getRadius


/*********************************************************************
 * Procedure calculateAreaAndCircumference is passed the radius of a *
 * circle and returns the circle's area and circumference.           *
 *********************************************************************/
void calculateAreaAndCircumference(double radius, double * area, 
				   double * circumference) {
  *area = PI * pow(radius, 2.0);
  *circumference = 2.0 * PI * radius;
} // end calculateAreaAndCircumference


/********************************************************************
 * Function displayCircleInformation is passed the radius, area, and *
 * circumference of a circle and displays this information to the    *
 * user.  Nothing is returned.                                       *
 *********************************************************************/
void displayCircleInformation(double radius, double area, double circumference) {
  
  printf("A circle with a radius of %3.2f has an area of %3.2f\n", radius, area);
  printf("and a circumference of %3.2f.\n", circumference);
  
} // end displayCircleInformation
