#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Circular buffe는 하나로 하고, 버퍼크기는 32 이하로 하고, 
#define BUFFER_SIZE 32
// 1,000개 이상의 데이터(숫자 등)를 생성 및 소비시킬 것
#define DATA_COUNT 1000

// Producer 쓰레드와 consumer 쓰레드를 각각 5개 이상 동시에 수행시킬것.
#define NUM_PRODUCERS 5
#define NUM_CONSUMERS 5
#define _BSD_SOURCE

int buffer[BUFFER_SIZE];
int counter = 0;
int in = 0;
int out = 0;

sem_t empty_slots;
sem_t filled_slots;
pthread_mutex_t mutex;

void producer(void *param) {
    int i;
    int data;

    for (i = 0; i < DATA_COUNT; i++) {
        // 랜덤으로 수를 발생시킵니다. 
        data = rand() % 1000;  

        // 버퍼 안이 비게 된다면 기다립니다.
        sem_wait(&empty_slots);

        // mutex를 이용해 lock 합니다. 
        pthread_mutex_lock(&mutex);  // Acquire the mutex lock

        // 버퍼에 데이터를 생성합니다. 
        buffer[in] = data;
        in = (in + 1) % BUFFER_SIZE;
        counter++;

        printf("Produced: %d , counter:%d\n", data, counter);

        // 락을 해제 합니다. 
        pthread_mutex_unlock(&mutex); 

        // 세마포어로 버퍼 안에 새로운 값이 생겼음을 소비자에게 알려줍니다. 
        sem_post(&filled_slots);  // Signal that a slot has been filled

        //sleep
        usleep(rand() % 1000000);
    }

    pthread_exit(0);
}

void consumer(void *param) {
    int i;
    int data;

    for (i = 0; i < DATA_COUNT; i++) {

        // 버퍼 안이 꽉차면 기다립니다.
        sem_wait(&filled_slots); 

        // mutex를 이용해 lock 합니다.
        pthread_mutex_lock(&mutex); 

        // 버퍼 안의 데이터를 소비합니다. 
        data = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;

        printf("Consumed: %d, Counter: %d\n", data, counter);

        // lock을 해제합니다.
        pthread_mutex_unlock(&mutex); 

        // 소비한 후 버퍼가 비었다면 반대쪽 생성자에 신호를 보내줍니다. 
        sem_post(&empty_slots);  // Signal that a slot has been emptied

        // Sleep
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

       
