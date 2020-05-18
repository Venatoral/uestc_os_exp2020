#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define PRODUCER_NUM 3
#define CONSUMER_NUM 4

#define BUF_SIZE 10
#define SOURCE_FILE "sources.txt"

FILE* f;

sem_t empty;
sem_t full;
pthread_mutex_t m;

int producerPtr = 0;
int consumePtr = 0;

char* buf[BUF_SIZE];
void randSleep() {
    sleep( rand() % 5);
}
void consume(void* param) {
    int n = (int)param;
    while(1) {
        // get resources empty --
        sem_wait(&full);
        pthread_mutex_lock(&m);

        int pos = consumePtr % BUF_SIZE;
        char* source = buf[pos];
        consumePtr++;
        // add 1 full postion
        sem_post(&empty);
        // show info
        printf("Consumer %d take product %s in position %d\n", n, source, pos);
        pthread_mutex_unlock(&m);
        randSleep();
    }
}

void produce(void* param) {
    int n = (int)param;
    while(1) {
        sem_wait(&empty);
        pthread_mutex_lock(&m);

        char source[32];
        // after reading file, have no source to produce
        if( feof(f))
            fseek(f, 0, SEEK_SET);
        fscanf(f, "%s\n", source);
        int pos = producerPtr % BUF_SIZE;
        buf[pos] = source;
        producerPtr ++;

        sem_post(&full);

        printf("Producer %d produce %s in position %d\n", n, source, pos);
        pthread_mutex_unlock(&m);
        sleep(2);
    }
}

int main() {
    // for randSleep()
    srand((unsigned)time(NULL));
    if( !( f = fopen(SOURCE_FILE, "r") ) ) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //init mutex
    if (pthread_mutex_init(&m, NULL) == -1) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
    // init empty $BUF_SIZE
    if ( sem_init(&empty, 0, BUF_SIZE) == -1 ) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    // init full 0
    if ( sem_init(&full, 0, 0) == -1) {
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