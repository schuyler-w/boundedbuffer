/*
 * The code is not part of the real application, and just used to 
 * illustrate the bounded-buffer problem using Semaphore and/or mutexes. 
 * Detailed requirements please refer to the assignment documentation.
 * 
*/

/*
 * Program 3
 * Name: Schuyler Wang
 * cssc1766@edoras.sdsu.edu
 * July 26, 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include "bbuffer.h"

// You may need to define more variables here
buffer_t buffer;
pthread_t consumer_tid[CONSUMERS];
pthread_t producer_tid[PRODUCERS];

// type sem_t variables for mutex, full, and empty statuses
sem_t *mutex;
sem_t *full;
sem_t *empty;

void initilization()
{
    // Initialize semaphores
    sem_unlink("mutex"); // unlinks semaphore mutex from system
    sem_unlink("full"); // unlinks semaphore full from system
    sem_unlink("empty"); // unlinks semaphore empty from system
    mutex = sem_open("mutex", O_CREAT | O_EXCL, 0644, 1); // creates and opens semaphore named mutex with flags
                                                                // O_CREAT - create if it doesn't already exist
                                                                // O_EXCL - ensures exclusive creation
                                                                // 0644 - permissions in octal
                                                                // 1 - initial semaphore value
    full = sem_open("full", O_CREAT | O_EXCL, 0644, 0); // creates and opens semaphore named full with initial value 0
    empty = sem_open("empty", O_CREAT | O_EXCL, 0644, BUFFER_SIZE); // creates and opens semaphore named empty
                                                                    // with initial value corresponding to BUFFER_SIZE
}

/* *
 * insert_item - thread safe function to insert items to the bounded buffer
 * @param item the value to be inserted
 * @return 0 in case of sucess -1 otherwise
 */
int insert_item(int item, long int id)
{
    sem_wait(empty); // Wait for an empty slot
    sem_wait(mutex); // Lock the critical region

    buffer.value[buffer.next_in] = item;
    printf("producer %ld: inserted item %d into buffer index %d\n", id, item, buffer.next_in);
    buffer.next_in = (buffer.next_in + 1) % BUFFER_SIZE;

    sem_post(mutex); // Unlock the critical region
    sem_post(full); // Signal that a slot is now full
    return 0;
}

/**
 * remove_item - thread safe function to remove items to the bounded buffer
 * @param item the address of the variable that the removed value will be written
 * @return 0 in case of sucess -1 otherwise
 */
int remove_item(int *item, long int id)
{
    sem_wait(full); // Wait for a full slot
    sem_wait(mutex); // Lock the critical region

    *item = buffer.value[buffer.next_out];
    printf("consumer %ld: removed item %d from buffer index %d\n", id, *item, buffer.next_out);
    buffer.value[buffer.next_out] = -1;
    buffer.next_out = (buffer.next_out + 1) % BUFFER_SIZE;

    sem_post(mutex); // Unlock the critical region
    sem_post(empty); // Signal that a slot is now empty
    return 0;
}

/**
 * producer - will iterate PRODUCER_ITERATION times and call the corresponding
 * function to insert an integer to the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple producer threads
 * @return nothing
 */
void *producer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("producer %ld started!\n", id);
    i = PRODUCER_ITERATIONS;
    while (i--) {
        sleep(rand() % 3);

        item = rand() % 10000;
        if (insert_item(item, id))
            printf("Error while inserting to buffer\n");
    }

    pthread_exit(0);
}

/**
 * consumer - will iterate CONSUMER_ITERATION times and call the corresponding
 * function to remove an integer from the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple consumer threads
 * @return nothing
 */
void *consumer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("consumer %ld started!\n", id);
    i = CONSUMER_ITERATIONS;
    while (i--) {
        sleep(rand() % 3);

        if (remove_item(&item, id))
            printf("Error while removing from buffer\n");
    }

    pthread_exit(0);
}
