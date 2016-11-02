/*  Edited by:   Vincent T. Mossman
    Programmer:  Mark Fienup
    File:        seqChromakey.c
    Compiled by: gcc -O3 -o hw10 hw10.c -lm
    Description: Performs chroma key/green-screen replace of a background
	image (swans.ppm) over a sequence of green-screen "loch ness" monster
	pictures (nessie001.ppm, nessie002.ppm, ..., nessie099.ppm).  This program
	hard-codes the file names which I know is bad...
*/
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
//#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define SIZE 10
#define MAX_FRAME 99
#define TRUE 1
#define FALSE 0
#define COLOR_TOLERANCE 150
// Typical Green-screen RGB value
#define BG_R 0
#define BG_G 255
#define BG_B 0


typedef struct {
  int width;
  int height;
  int frameNum;
  char * fileName;
  unsigned char ** r;
  unsigned char ** g;
  unsigned char ** b;
} PICTURE;


// prototypes
unsigned char ** allocate2DArray(int rows, int columns);
void free2DArray(unsigned char ** array2D, int rows, int columns);
char * generateFileName(char * baseStr, int frameNumber, char * extStr);
PICTURE readPicture(char * fileName, int frameNumber);
void writePicture(PICTURE picture, char * fileName);
void chromakey(PICTURE picture, PICTURE bgPicture);

int main(int argc, char * argv[]) {
  PICTURE bgPicture;
  PICTURE fgPicture;
  int frameNumber;
  char * fileName;

  // mpi stuff
  int myID;
  int p;
  int RootProcess=0;
  int numPics;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myID);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  if ( myID == RootProcess ) {
    if (argc != 5) {
      printf("Usage: %s <complete path of folder> <name of new background file> <base name of green-screen pictures> <# of green-screen picture>\n", argv[0], argv[1]);
      return(0);
    } // end if myID
  } // end if argc

  sscanf(argv[4],"%d",&numPics);

  // set full directory for file I/O
  char swansFileName[1024];
  strcpy(swansFileName, argv[1]);
  strcat(swansFileName, "/");
  strcat(swansFileName, argv[2]);

  char nessieFileName[1024];
  strcpy(nessieFileName, argv[1]);
  strcat(nessieFileName, "/");
  strcat(nessieFileName, argv[3]);

  char frameFileName[1024];
  strcpy(frameFileName, argv[1]);
  strcat(frameFileName, "/frame");

  bgPicture = readPicture(swansFileName,0);

  // do stuff
  for (frameNumber = myID+1; frameNumber <= numPics; frameNumber+=p) {
    fileName = generateFileName(nessieFileName,frameNumber,".ppm");
    fgPicture = readPicture(fileName, frameNumber);
    chromakey(fgPicture, bgPicture);
    fileName = generateFileName(frameFileName,frameNumber,".ppm");
    writePicture(fgPicture, fileName);
  } // end for

  MPI_Finalize();
} // end main



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
