#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 5   // rounded buffer size

#define PRODUCERS 3  // number of producers
#define CONSUMERS 5  // number of consumers

#define ITERATIONS 30   // number of actions producers/consumers perform
#define PRODUCER_ITERATIONS (ITERATIONS / PRODUCERS)  //number of actions each producer performs
#define CONSUMER_ITERATIONS (ITERATIONS / CONSUMERS) //number of actions each consumer performs

typedef struct {
    int value[BUFFER_SIZE];
    int next_in, next_out;
} buffer_t;    // struct of bounded buffer

extern buffer_t buffer;   //global variable: rounded buffer

extern pthread_t consumer_tid[CONSUMERS]; // producer threads
extern pthread_t producer_tid[PRODUCERS]; // consumer threads

extern sem_t *mutex; // Mutex semaphore
extern sem_t *full;  // Full semaphore
extern sem_t *empty; // Empty semaphore

void *producer(void *param);
void *consumer(void *param);
void initilization();
void cleanup();

int insert_item(int item, long int id);
int remove_item(int *item, long int id);
