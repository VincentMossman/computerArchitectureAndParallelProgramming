/* File:  makeReverseString.c
   Compile by: gcc -o reverseString makeReverseString.c
   Run by: ./age Today is a good day to learn C
   Output:  C learn to day good a is Today
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {

  int i, inputLength = 0;
  char * reverseInput;
  
  for(i = argc - 1; i > 0; i--) {
    inputLength = inputLength + strlen(argv[i]) + 1;
  } // end for
  
  reverseInput = (char *) malloc(sizeof(char) * (inputLength) + 1);

  for(i = argc - 1; i > 0; i--) {
    strcat(reverseInput, argv[i]);
    strcat(reverseInput, " ");
  } // end for
  
  printf("%s\n", reverseInput);

  free(reverseInput);
 
  return 0;

} // end main
