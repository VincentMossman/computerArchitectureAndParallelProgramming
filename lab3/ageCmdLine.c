/* File:  ageCmdLine.c
   Compile by: gcc -o age ageCmdLine.c
   Run by: ./age Bob 13
   Output:  Bob your age is 13.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int SIZE = 100;

int main(int argc, char * argv[]) {
  int age;
  char * name;

  if (argc != 3) {
    printf("Usage %s firstName #age\n", argv[0]);
    exit(-1);
  }

  name = (char *) malloc(sizeof(char)*(strlen(argv[1])+1));
  strcpy(name, argv[1]);

  sscanf(argv[2],"%d",&age);

  printf("%s your age is %d.\n", name, age);

  free(name);   // deallocates the array

  return 0;

} // end main
