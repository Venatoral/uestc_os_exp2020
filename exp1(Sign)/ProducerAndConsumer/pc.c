#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define PRODUCER_NUM 10
#define CONSUMER_NUM 5

#define FOOD_NUM 20

sem_t empty;
sem_t full;

void randSleep() {
    sleep( rand() % 5);
}
void consume(void* param) {
    int n = (int)param;
    while(1) {
        printf("Consumer.%d start consume\n", n);
        // get resources empty --
        sem_wait(&empty);
        // add 1 full postion
        sem_post(&full);
        // show info
        int value = 0;
        sem_getvalue(&empty, &value);
        printf("Consumer.%d finish consuming => %d food left\n", n, value);
        randSleep();
    }
}

void produce(void* param) {
    int n = (int)param;
    while(1) {
        printf("Producer.%d start produce\n", n);
        sem_wait(&full);
        sem_post(&empty);
        int value = 0;
        sem_getvalue(&empty, &value);
        printf("Producer.%d produce 1 food => %d food\n", n, value);
        randSleep();
    }
}

int main() {
    // for randSleep()
    srand((unsigned)time(NULL));
    // init empty $FOOD_NUM
    if ( sem_init(&empty, 0, FOOD_NUM) == -1 ) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    // init full 0
    if (sem_init(&full, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    pthread_t producer[PRODUCER_NUM];
    pthread_t consumer[CONSUMER_NUM];
    // Create consumers
    for(int i = 0; i < CONSUMER_NUM; i++) {
        if( pthread_create(consumer + i, 0, (void*)&consume, (void*)i) == -1 ) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    // Create producers
    for (int i = 0; i < PRODUCER_NUM; i++) {
        if (pthread_create(producer + i, 0, (void *)&produce, (void*)i) == -1) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // wait consumer to end
    for( int i = 0; i < CONSUMER_NUM; i++) {
        pthread_join(producer[i], NULL);
    }
    
    // wait producer to end
    for(int i = 0;i < PRODUCER_NUM; i++) {
        pthread_join(consumer[i], NULL);
    }

    return 0;
}