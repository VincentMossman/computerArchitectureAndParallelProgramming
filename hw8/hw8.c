/*  Edited by:   Vincent T. Mossman
    Programmer:  Mark Fienup
    File:        seqChromakey.c
    Compiled by: gcc -O3 -o hw8 hw8.c -lpthread -lm
    Description: Performs chroma key/green-screen replace of a background
	image (swans.ppm) over a sequence of green-screen "loch ness" monster
	pictures (nessie001.ppm, nessie002.ppm, ..., nessie099.ppm).  This program
	hard-codes the file names which I know is bad...
*/
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define MAX_FRAME 99
#define TRUE 1
#define FALSE 0
#define COLOR_TOLERANCE 150
// Typical Green-screen RGB value
#define BG_R 0
#define BG_G 255
#define BG_B 0

#define MAX_PRODUCER_SLEEP 10
#define MAX_CONSUMER_SLEEP 5
#define MAX_DURATION 100


typedef struct {
  int width;
  int height;
  int frameNum;
  char * fileName;
  unsigned char ** r;
  unsigned char ** g;
  unsigned char ** b;
} PICTURE;

// Global variables
int nextFrameNumber = 0;
int consumedCount = 0;

// Global Bounded Buffer
PICTURE buffer[SIZE];
int count = 0;
int front = -1;
int rear = 0;

pthread_mutex_t lock;
pthread_cond_t nonFull;
pthread_cond_t nonEmpty;

// prototypes
unsigned char ** allocate2DArray(int rows, int columns);
void free2DArray(unsigned char ** array2D, int rows, int columns);
char * generateFileName(char * baseStr, int frameNumber, char * extStr);
PICTURE readPicture(char * fileName, int frameNumber);
void writePicture(PICTURE picture, char * fileName);
void chromakey(PICTURE picture, PICTURE bgPicture);

void  *producerWork(void *);
void  *consumerWork(void *);
void bufferAdd(PICTURE);
PICTURE bufferRemove();


int main(int argc, char * argv[]) {

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&nonFull, NULL);
  pthread_cond_init(&nonEmpty, NULL);

  pthread_t threadProdHandle[64];
  pthread_attr_t attrProd[64];
  pthread_t threadConHandle[64];
  pthread_attr_t attrCon[64];

  int i,numberOfProducerThreads,numberOfConsumerThreads;

  if (argc != 3) {
    printf("usage: %s <integer number of producter and consumer threads>\n",
     argv[0]);
    exit(1);
  } // end if

  sscanf(argv[1], "%d", &numberOfProducerThreads);
  sscanf(argv[2], "%d", &numberOfConsumerThreads);

  PICTURE bgPicture;

  bgPicture = readPicture("swans.ppm",0);

  for (i=0; i < numberOfProducerThreads; i++) {
    pthread_attr_init(&attrProd[i]);
    pthread_attr_setscope(&attrProd[i], PTHREAD_SCOPE_SYSTEM);
    // create producer threads
    pthread_create(&threadProdHandle[i], &attrProd[i], producerWork, &i);
  } // end for

  for (i=0; i < numberOfConsumerThreads; i++) {
    pthread_attr_init(&attrCon[i]);
    pthread_attr_setscope(&attrCon[i], PTHREAD_SCOPE_SYSTEM);
    // create consumer threads
    pthread_create(&threadConHandle[i], &attrCon[i], consumerWork, &i);
  } // end for

  for (i=0; i < numberOfProducerThreads; i++) {
    pthread_join(threadProdHandle[i], (void **) NULL);
  } // end for
  for (i=0; i < numberOfConsumerThreads; i++) {
    pthread_join(threadConHandle[i], (void **) NULL);
  } // end for
  //printf("After join in main!\n");

} // end main

void  *producerWork(void * args) {

  PICTURE bgPicture = readPicture("swans.ppm",0);
  PICTURE fgPicture;
  int frameNumber;
  char * fileName;

  while (nextFrameNumber < MAX_FRAME) {
    pthread_mutex_lock(&lock);
    nextFrameNumber++;
    frameNumber = nextFrameNumber;
    pthread_mutex_unlock(&lock);

    fileName = generateFileName("nessie",frameNumber,".ppm");
    bufferAdd(readPicture(fileName,frameNumber));
  }

} // end producerWork

void  *consumerWork(void * args) {

  PICTURE bgPicture = readPicture("swans.ppm",0);
  PICTURE fgPicture;
  char * fileName;

  while (consumedCount < MAX_FRAME) {
    pthread_mutex_lock(&lock);
    consumedCount++;
    pthread_mutex_unlock(&lock);

    fgPicture = bufferRemove();
    chromakey(fgPicture, bgPicture);
    fileName = generateFileName("frame",consumedCount,".ppm");
    writePicture(fgPicture, fileName);
  } // end while

} // end consumerWork

char * generateFileName(char * baseStr, int frameNumber, char * extStr) {
  int length;
  char * fileName;
  char frameNumberStr[4];


  length = strlen(baseStr) + 3 + strlen(extStr);
  fileName = (char *) malloc(sizeof(char)*(length+1));

  frameNumberStr[0] = '0' + frameNumber/100;
  frameNumber = frameNumber%100;
  frameNumberStr[1] = '0' + frameNumber/10;
  frameNumber = frameNumber%10;
  frameNumberStr[2] = '0'+frameNumber;
  frameNumberStr[3] = '\0';

  fileName[0] = '\0';
  strncat(fileName, baseStr, strlen(baseStr));
  strncat(fileName, frameNumberStr, strlen(frameNumberStr));
  strncat(fileName, extStr, strlen(extStr));

  return fileName;

} // end generateFileName

/*******************************************************************
 * Function allocate2DArray dynamically allocates a 2D array of
 * size rows x columns, and returns it.
 ********************************************************************/
unsigned char ** allocate2DArray(int rows, int columns) {
  unsigned char ** local2DArray;
  int r;

  local2DArray = (unsigned char **) malloc(sizeof(unsigned char *)*rows);

  for (r=0; r < rows; r++) {
    local2DArray[r] = (unsigned char *) malloc(sizeof(unsigned char)*columns);
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


void chromakey(PICTURE picture, PICTURE bgPicture) {
  int row, col;
  int diffR, diffG, diffB, colorDistance;

  for (row = 0; row < picture.height; row++) {

    for (col = 0; col < picture.width; col++) {
      diffR = picture.r[row][col] - BG_R;
      diffG = picture.g[row][col] -BG_G;
      diffB = picture.b[row][col] - BG_B;
      colorDistance = (int) sqrt(diffR*diffR + diffG*diffG + diffB*diffB);
      if (colorDistance < COLOR_TOLERANCE) {
	picture.r[row][col] = bgPicture.r[row][col];
	picture.g[row][col] = bgPicture.g[row][col];
	picture.b[row][col] = bgPicture.b[row][col];
      } // end if

    } // end for(col...

  } // end for(row...

} // end chromakey


PICTURE readPicture(char * fileName, int frameNumber) {
  PICTURE picture;
  FILE * pictureFile;
  char buffer[200];
  int row, col;

  picture.fileName = fileName;
  picture.frameNum = frameNumber;
  if ((pictureFile = fopen(fileName, "r")) == NULL) {
    printf("%s cannot be opened for reading\n", fileName);
    exit(-1);
  } // end if
  fgets(buffer, 200, pictureFile); // file type P3
  fgets(buffer, 200, pictureFile); // comment line
  fgets(buffer, 200, pictureFile);
  sscanf(buffer, "%d %d",&(picture.width),&(picture.height));
  fgets(buffer, 200, pictureFile); // max RGB value

  picture.r = allocate2DArray(picture.height, picture.width);
  picture.b = allocate2DArray(picture.height, picture.width);
  picture.g = allocate2DArray(picture.height, picture.width);

  for (row = 0; row < picture.height; row++) {

    for (col = 0; col < picture.width; col++) {
      fscanf(pictureFile,"%d",&(picture.r[row][col]));
      fscanf(pictureFile,"%d",&(picture.g[row][col]));
      fscanf(pictureFile,"%d",&(picture.b[row][col]));

    } // end for(col...

  } // end for(row...

  fclose(pictureFile);
  return picture;
} // end readPicture

void writePicture(PICTURE picture, char * fileName) {
  FILE * pictureFile;
  char buffer[200];
  int row, col;

  if ((pictureFile = fopen(fileName, "w")) == NULL) {
    printf("%s cannot be opened for writing\n", fileName);
    exit(-1);
  } // end if
  fprintf(pictureFile,"%s\n","P3"); // file type P3
  fprintf(pictureFile,"# file %s\n", fileName); // comment line
  fprintf(pictureFile, "%d %d\n", picture.width, picture.height);
  fprintf(pictureFile,"%d\n",255);

  for (row = 0; row < picture.height; row++) {

    for (col = 0; col < picture.width; col++) {
      fprintf(pictureFile,"%3d %3d %3d  ",picture.r[row][col],
	      picture.g[row][col], picture.b[row][col]);

    } // end for(col...
    fprintf(pictureFile,"\n");
  } // end for(row...

  fclose(pictureFile);

  free2DArray(picture.r, picture.height, picture.width);
  free2DArray(picture.g, picture.height, picture.width);
  free2DArray(picture.b, picture.height, picture.width);

} // end writePicture

void bufferAdd(PICTURE item) {

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

PICTURE bufferRemove() {
  PICTURE returnValue;

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
