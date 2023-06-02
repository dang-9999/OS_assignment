#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 32
#define DATA_COUNT 1000
#define NUM_PRODUCERS 5
#define NUM_CONSUMERS 5

int buffer[BUFFER_SIZE];
int counter = 0;
int in = 0;
int out = 0;

sem_t empty_slots;
sem_t filled_slots;
pthread_mutex_t mutex;

void *producer(void *param) {
    int i;
    int data;

    for (i = 0; i < DATA_COUNT; i++) {
        data = rand() % 1000;  // Generate random data

        sem_wait(&empty_slots);  // Wait for an empty slot in the buffer
        pthread_mutex_lock(&mutex);  // Acquire the mutex lock

        // Add data to the buffer
        buffer[in] = data;
        in = (in + 1) % BUFFER_SIZE;
        counter++;

        printf("Produced: %d\n", data);

        pthread_mutex_unlock(&mutex);  // Release the mutex lock
        sem_post(&filled_slots);  // Signal that a slot has been filled

        // Sleep for a random period of time
        usleep(rand() % 1000000);
    }

    pthread_exit(0);
}

void *consumer(void *param) {
    int i;
    int data;

    for (i = 0; i < DATA_COUNT; i++) {
        sem_wait(&filled_slots);  // Wait for a filled slot in the buffer
        pthread_mutex_lock(&mutex);  // Acquire the mutex lock

        // Remove data from the buffer
        data = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;

        printf("Consumed: %d\n", data);

        pthread_mutex_unlock(&mutex);  // Release the mutex lock
        sem_post(&empty_slots);  // Signal that a slot has been emptied

        // Sleep for a random period of time
        usleep(rand() % 1000000);
    }

    pthread_exit(0);
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int i;

    // Initialize semaphores and mutex
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&filled_slots, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create producer threads
    for (i = 0; i < NUM_PRODUCERS; i++)
        pthread_create(&producers[i], NULL, producer, NULL);

    // Create consumer threads
    for (i = 0; i < NUM_CONSUMERS; i++)
        pthread_create(&consumers[i], NULL, consumer, NULL);

    // Wait for producer threads to finish
    for (i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producers[i], NULL);

    // Wait for consumer threads to finish
    for (i = 0; i < NUM_CONSUMERS; i++)
    pthread_join(consumers[i], NULL);

    // Clean up resources
    sem_destroy(&empty_slots);
    sem_destroy(&filled_slots);
    pthread_mutex_destroy(&mutex);

    return 0;
}

       
