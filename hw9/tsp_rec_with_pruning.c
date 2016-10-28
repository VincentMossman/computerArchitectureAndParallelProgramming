/************************** tsp_rec_with_pruning.c ****************************
Programmer:  Mark Fienup
Compile by:  gcc -O3 -o tsp_rec_with_pruning tsp_rec_with_pruning.c
Description:  This program performs a backtracking solution to the traveling
salesperson problem (TSP).  It's promising function calculates a bound by summing 
the minimum edges leaving vertices not on the partial tour.  
Note:  Run by redirecting file containing graph as standard input.
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

#define INFINITY 1000000
#define FALSE 0
#define TRUE 1

#define BOOL int
#define MAX 100

// prototypes	
void ReadArray(FILE * digraph_file, int E [][MAX], int * n);
void PrintArray(int E [][MAX], int n);
void tspInitialize(int included[], int path[]);
void tsp(int level, int pathLength);
BOOL promising(int nextVertex, int pathLength, int level);

// global variables
int E [MAX][MAX];
int included [MAX];
int partialPath [MAX + 1];
int n; 

// Generally start with intMax, but if you know a bound...
//int bestLength = intMax;
int bestLength = INFINITY;  
int bestPath [ MAX + 1 ];

int main(int argc, char* argv[]) {
  FILE* digraph_file;
  double start, finish;
  int i;

  if (argc != 2) {
    printf("Usage: %s <graphFile>\n", argv[0]);
    exit(1);
  } // end if

  digraph_file = fopen(argv[1], "r");
  if (digraph_file == NULL) {
    fprintf(stderr, "Can't open %s\n", argv[1]);
    printf("Usage: %s <graphFile.txt>\n", argv[0]);
    exit(1);
  } // end if

  ReadArray(digraph_file, E, &n);
  printf("The adjacency matrix is:\n");
  PrintArray (E, n);

  GET_TIME(start);

  tspInitialize(included, partialPath);
  tsp(1, 0);

  GET_TIME(finish);
  printf("\nElapsed time = %e seconds\n", finish-start);

  printf("Best tour length is %d\n", bestLength);
  printf("The best tour is:  ");
 
  for (i = 0; i <= n; i++) {
    printf("%d ", bestPath[i]);
  } // end for (i...
  printf("\n");

} // end main


void tsp(int level, int pathLength) {
  int completePathLength;
  int nextVertex;
  int i;

  //cout << "tsp:: level = " << level << "    pathLength = " << pathLength << endl;

  for (nextVertex = 1; nextVertex < n; nextVertex++) {
    // cout << "for nextVertex = " << nextVertex << " level = " << level << endl;
    if (!included[nextVertex]) {  // only consider next vertices that are not 
                                  // already on the partial tour
      if (level == n-1){          // for a complete tour see if it is the best so far
	completePathLength = pathLength + E[partialPath[level-1]][nextVertex]
	                                + E[nextVertex][0];
	if (bestLength > completePathLength) {
	  bestLength = completePathLength;
	  for (i = 0; i < level; i++) {
	    bestPath[i] = partialPath[i];
	  } // end for(i...
	  bestPath[level] = nextVertex;
	  bestPath[level+1] = 0;
	  //cout << "new best length " << bestLength << endl;
	} // end (bestLength...
      } else if (promising(nextVertex, pathLength, level)) { // only check promising nodes
	included[nextVertex] = TRUE;      // update global arrays to reflect child
	partialPath[level] = nextVertex;
	tsp (level+1, pathLength + E[partialPath[level-1]][nextVertex]);
	included[nextVertex] = FALSE;    // change back to the parent's state
      } // end if (level...
    } // end if (!included[nextVertex]...
  } // end for (nextVertex...

} // end tsp

// this promising function calculates a bound by summing the minimum edges leaving 
// vertices not on the partial tour
int promising(int nextVertex, int pathLength, int level) {
  int bound;
  int i, test;
  int minEdge;

  // update the partial path to reflect the child
  pathLength += E[partialPath[level-1]][nextVertex];
  included[nextVertex] = TRUE;

  // find the minimum edge leaving the vertex on the end of the partial tour.
  // This vertex cannot go back to vertex 0 (the start) because there are other 
  // vertices that must be visited first.
  minEdge = INFINITY;
  for (i = 1; i < n; i++) {
    if (!included[i] && E[nextVertex][i] < minEdge) {
      minEdge = E[nextVertex][i];
    } // end if 
  } // end for (i
  bound = pathLength + minEdge;

  // temporarily set to FALSE so vertex 0 is included in the minimum calculation
  included[0] = FALSE;
  for (test = 1; test < n; test++) {
    if (!included[test]) {
      minEdge = INFINITY;
      for (i = 0; i < n; i++) {
	if (!included[i] && test != i && E[test][i] < minEdge) {
	  minEdge = E[test][i];
	} // end if 
      } // end for (i
      bound += minEdge;
    } // end if (!included....
  } // end for (test...
  
  included[0] = TRUE;
  included[nextVertex] = FALSE;

  //cout << "child: " << nextVertex << "  bound = " << bound;
  if (bound < bestLength) {
    //cout << " PROMISING" << endl;
    return TRUE;
  } else {
    //cout << " NOT PROMISING" << endl;
    return FALSE;
  } // end if
} // end promising


// Initial the partial tour to only include vertex 0
void   tspInitialize(int included[], int path[]) {
  int i;

  included[0] = TRUE;
  path[0] = 0;

  for (i = 1; i < n; i++) {
    included[i] = FALSE;
  } // end for (i
} // end tspInitialize

// Read the number of vertices in the graph followed by the adjacency matix.
void ReadArray(FILE * digraph_file, int E [][MAX], int * n) {
  int r, c;

  fscanf(digraph_file, "%d", n);
  for (r = 0; r < *n; r++) {
    for (c = 0; c < *n; c++) {
      fscanf(digraph_file, "%d", &(E[r][c]));
    } // end for (c
  } // end for (r
} // end ReadArray


// Prints the adjacency matrix 
void PrintArray(int E [][MAX], int n) {
  int r, c;

  for (r = 0; r < n; r++) {
    for (c = 0; c < n; c++) {
      printf("%7d  ", E[r][c]);
    } // end for (c
    printf("\n");
  } // end for (r
} // end ReadArray
