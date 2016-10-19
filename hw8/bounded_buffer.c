/*  Programmer:  Mark Fienup
    File:        bounded_buffer.c
    Compiled by: gcc bounded_buffer.c -lpthread -lm
    Description:  Bounded buffer using pthread condition variables to avoid over or underflowing the bounded buffer.
*/
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 5
#define MAX_PRODUCER_SLEEP 10
#define MAX_CONSUMER_SLEEP 5
#define MAX_DURATION 100

// Global Bounded Buffer
int buffer[SIZE];
int count = 0;
int front = -1;
int rear = 0;

pthread_mutex_t lock;
pthread_cond_t nonFull;
pthread_cond_t nonEmpty;

void  *producerWork(void *);
void  *consumerWork(void *);
void bufferAdd(int);
int bufferRemove();


int main(int argc, char * argv[]) {

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&nonFull, NULL);
  pthread_cond_init(&nonEmpty, NULL);

  pthread_t threadHandle[64];
  pthread_attr_t attr[64];  // One might be enough???

  int i,numberOfThreads;

  if (argc != 2) {
    printf("usage: %s <integer number of producter and consumer threads>\n",
	   argv[0]);
    exit(1);
  } // end if

  sscanf(argv[1], "%d", &numberOfThreads);

  srand(5);

  for (i=0; i < numberOfThreads; i++) {
    pthread_attr_init(&attr[i]);
    pthread_attr_setscope(&attr[i], PTHREAD_SCOPE_SYSTEM);
    if (i % 2 == 0) {
      // create producer threads
      pthread_create(&threadHandle[i], &attr[i], producerWork, &i);
    } else {
      pthread_create(&threadHandle[i], &attr[i], consumerWork, &i);
    } // end if
  } // end for

    for (i=0; i < numberOfThreads; i++) {
      pthread_join(threadHandle[i], (void **) NULL);
    } // end for
    printf("After join in main!\n");

} // end main


void  *producerWork(void * args) {

  int threadId;
  int counter = 0;
  unsigned int sleepAmt;

  threadId = *((int *) args);

  while (counter < MAX_DURATION) {
    sleepAmt = rand() % MAX_PRODUCER_SLEEP;
    sleep(sleepAmt);
    printf("Producer try to add when count = %d\n", count);
    bufferAdd(threadId);
    printf("Producer added\n");
    counter++;
  } // end while


} // end producerWork

void  *consumerWork(void * args) {

  int threadId;
  int counter = 0;
  unsigned int sleepAmt;

  threadId = *((int *) args);

  while (counter < MAX_DURATION) {
    sleepAmt = rand() % MAX_CONSUMER_SLEEP;
    sleep(sleepAmt);
    printf("Consumer trying to remove when count = %d\n", count);
    bufferRemove(threadId);
    printf("Consumer removed\n");
    counter++;
  } // end while

} // end consumerWork


void bufferAdd(int item) {

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

int bufferRemove() {
  int returnValue;

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
