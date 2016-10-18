/*  Programmer:  Mark Fienup
    File:        maddE.c
    Compiled by: gcc -O3 -o madd maddE.c -lpthread -lm
    Description:  Bounded buffer using pthread condition variables to 
    avoid over or underflowing the bounded buffer.
*/
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"  // Textbook timer MACROs


#define SIZE 20    // # of slots in the bounded buffer
#define TRUE 1
#define FALSE 0
#define BOOL int

typedef struct {
  int rowStart;
  int rowEnd;
} WORK;

// Global Bounded Buffer
WORK buffer[SIZE];
int count = 0;
int front = -1;
int rear = 0;
pthread_mutex_t lock;
pthread_cond_t nonFull;
pthread_cond_t nonEmpty;

void  *producerWork(void *);
void  *consumerWork(void *);
void bufferAdd(WORK);
WORK bufferRemove();

// Global next row # to be produced
pthread_mutex_t nextRowNumberLock;
int nextRowNumber = 0;

// Global count of rows consumed
pthread_mutex_t rowsConsumedCountLock;
int rowsConsumedCount = 0;
pthread_mutex_t allRowsConsumedLock;

// prototypes
double ** allocate2DArray(int rows, int columns);
void free2DArray(unsigned char ** array2D, int rows, int columns);
BOOL equal2DArrays(int rows, int columns, double ** array1, double ** array2,
                   double tolerance);
void matrixAddition(int rows1, int columns1, double ** array1, double ** array2,
                    double ** arraySum);
void print2DArray(int rows, int columns, double ** array2D);
void generateRandomRow(double * rowArray, int length, double min, 
		       double max, unsigned int * seed);
void addRows(double * row1, double * row2, double * rowSum, int length);

// Global variables
double ** A;
double ** B;
double ** Sum;
int rows, columns;

int main(int argc, char * argv[]) {
  int numberOfProducerThreads, numberOfConsumerThreads, numberOfThreads;
  long i;
  double initializationTime, startTime, endTime, seqTime, parallelTime;
  pthread_t * threadHandles;
  int errorCode;
  double ** Sum_seq;
  double tolerance;

  if (argc != 5) {
    printf("usage: %s <# rows> <# columns> <# producer threads> <# consumer threads>\n",
	   argv[0]);
    exit(1);
  } // end if

  sscanf(argv[1], "%d", &rows);
  sscanf(argv[2], "%d", &columns);
  sscanf(argv[3], "%d", &numberOfProducerThreads);
  sscanf(argv[4], "%d", &numberOfConsumerThreads);

  // initialize mutexes
  pthread_mutex_init(&nextRowNumberLock, NULL);
  pthread_mutex_init(&rowsConsumedCountLock, NULL);

  // Bounded Buffer initialization
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&nonFull, NULL);
  pthread_cond_init(&nonEmpty, NULL);

  // allocate threadHandles and attributes
  numberOfThreads = numberOfProducerThreads + numberOfConsumerThreads;
  threadHandles = (pthread_t *) malloc(numberOfThreads*sizeof(pthread_t));

  srand(5);
  A = allocate2DArray(rows, columns);
  B = allocate2DArray(rows, columns);
  Sum = allocate2DArray(rows, columns);
  Sum_seq = allocate2DArray(rows, columns);

  printf("Array allocations done\n");
  GET_TIME(startTime);

  // create Producer threads
  for (i=0; i < numberOfProducerThreads; i++) {
    pthread_create(&threadHandles[i], NULL, producerWork, (void*) i);
  } // end for

  pthread_mutex_lock(&allRowsConsumedLock);
    
  // create Consumer threads
  for (i=numberOfProducerThreads; i < numberOfThreads; i++) {
    pthread_create(&threadHandles[i], NULL, consumerWork, (void*) i);
  } // end for
    
  for (i=0; i < numberOfProducerThreads; i++) {
    pthread_join(threadHandles[i], (void **) NULL);
  } // end for
  // printf("After join of Produceers in main!\n");

  pthread_mutex_lock(&allRowsConsumedLock);

  GET_TIME(endTime);
  printf("Parallel Time with %d Producers and %d Consumers = %1.3f\n", 
	 numberOfProducerThreads, numberOfConsumerThreads, endTime-startTime);

  matrixAddition(rows, columns, A, B, Sum_seq);

  tolerance = 0.0000001;
  if (equal2DArrays(rows, columns, Sum, Sum_seq, tolerance)) {
    printf("Arrays match with tolerance of %.10f\n", tolerance);
  } else {
    printf("Arrays DON'T match with tolerance of %.10f\n", tolerance);
  } // end if

  // if small enough, print to screen
  if (rows < 10 && columns < 10) {
    printf("Matrix Sum sequential:\n");
    print2DArray(rows, columns, Sum_seq);
    printf("\nMatrix Sum parallel:\n");
    print2DArray(rows, columns, Sum);
  } // end if

  return 0;


    
} // end main


void  *producerWork(void * args) {
  
  long threadId = (long) args;
  int rowNumber;
  WORK blockOfWork;
  unsigned int seed;

  seed = (threadId+1)*1000;

  while (TRUE) {
    pthread_mutex_lock(&nextRowNumberLock);
    rowNumber = nextRowNumber;
    nextRowNumber++;
    pthread_mutex_unlock(&nextRowNumberLock);
    if (rowNumber >= rows) {
      break;
    } // end if

    generateRandomRow(A[rowNumber], columns, 0.0, +5.0, &seed);
    generateRandomRow(B[rowNumber], columns, 0.0, +5.0, &seed);

    blockOfWork.rowStart=rowNumber;
    blockOfWork.rowEnd = rowNumber;

    bufferAdd(blockOfWork);

  } // end while
  

} // end producerWork

void  *consumerWork(void * args) {
  
  long threadId = (long) args;
  int rowNumber;
  int counter = 0;
  WORK blockOfWork;

  while (TRUE) {
    
    blockOfWork = bufferRemove(threadId);

    rowNumber = blockOfWork.rowStart;
    addRows(A[rowNumber], B[rowNumber], Sum[rowNumber], columns);

    pthread_mutex_lock(&rowsConsumedCountLock);
    rowsConsumedCount++;
    if (rowsConsumedCount >= rows) {
      pthread_mutex_unlock(&allRowsConsumedLock);
    }  
    pthread_mutex_unlock(&rowsConsumedCountLock);
  } // end while

} // end consumerWork


void bufferAdd(WORK item) {

  pthread_mutex_lock(&lock);
  while(count == SIZE) {
    while(pthread_cond_wait(&nonFull, &lock) != 0);
  } // end while
  if (count == 0) {
    front = 0;
    rear = 0;
  } else {
    rear = (rear + 1) % SIZE;
  } // end if
  buffer[rear] = item;
  count++;
  pthread_cond_signal(&nonEmpty);
  pthread_mutex_unlock(&lock);

} // end bufferAdd

WORK bufferRemove() {
  WORK returnValue;

  pthread_mutex_lock(&lock);
  while(count == 0) {
    while(pthread_cond_wait(&nonEmpty, &lock) != 0);
  } // end while
  returnValue = buffer[front];
  front = (front + 1) % SIZE;
  count--;
  pthread_cond_signal(&nonFull);
  pthread_mutex_unlock(&lock);
  return returnValue;

} // end bufferRemove

   
/*******************************************************************
 * Function generatesRandomRow randomly assigns doubles to a 1D array of
 * size length.
 ********************************************************************/
void generateRandomRow(double * rowArray, int length, double min, 
		       double max, unsigned int * seed) {
  int index;
  double range, div;

  for (index = 0; index < length; index++) {
    range = max - min;
    div = RAND_MAX / range;
    rowArray[index] = min + (rand_r(seed) / div);
  } // end for (index...
 
} // generateRandomRow



/*******************************************************************
 * Function addRows adds two 1D  arrays of size length, and returns
 * their sum in the 1D array rowSum.
 ********************************************************************/
void addRows(double * row1, double * row2, double * rowSum, int length) {
  int index;

  for (index = 0; index < length; index++) {
    rowSum[index] = row1[index] + row2[index];
  } // end for (index...

} // end addRows


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
 * Function free2DArray dynamically deallocates a 2D array of
 * size rows x columns, and returns it.
 ********************************************************************/
void free2DArray(unsigned char ** array2D, int rows, int columns) {
  unsigned char ** local2DArray;
  int r;

  for (r=0; r < rows; r++) {
    free(array2D[r]);
  } // end for
  free(array2D);

} // end free2DArray


/*******************************************************************
 * Function matrixAddition - passed the dimenstions of two 2D arrays,
 * two 2D arrays, and returns the resulting 2D sum array.
 ********************************************************************/
void matrixAddition(int rows, int columns, double ** array1, double ** array2,
                    double ** arraySum) {
  int r, c;

  for (r = 0; r < rows; r++) {
    for (c = 0; c < columns; c++) {
      arraySum[r][c] = array1[r][c] + array2[r][c];
    } // end for (c...
  } // end for (r...

} // end matrixAddition


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
