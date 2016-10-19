/*
File:		age.c
Compile by:	gcc -o age age.c
Run by:		./age
*/

#include <stdlib.h>
#include <stdio.h>

const int SIZE = 100;

int main(int argc, char * argv[]){
	char name[SIZE];
	int age;

	printf("Enter your name: ");
	scanf("%s", name);

	printf("Enter your age: ");
	scanf("%d", &age);

	printf("%s your age is %d.\n", name, age);

	return 0;

}	// end main
