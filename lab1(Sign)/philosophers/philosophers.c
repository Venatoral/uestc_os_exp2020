#include <linux/sem.h>
#include <linux/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define PHILOSOPHER_NUM 5
#define CHOPSTICK_NUM 5

pthread_mutex_t chopstick[PHILOSOPHER_NUM];

void think() {
    int num = random() % 5;
    sleep(num);
}
void eat(int num) {
    while (1) {
        printf("%d is thinking...\n", num);
        //think();
        printf("%d want to eat\n", num);
        
        
        int wantChops[2] = {num, (num + 1) % 5};
        if(num % 2 == 0) {
            int tmp = wantChops[0];
            wantChops[0] = wantChops[1];
            wantChops[1] = tmp;
        }

        /**
         * Below will cause dead lock
         * ==========================
         * wantChop[2] = {num, (num + 1) % 5};
         * ==========================
         **/
        pthread_mutex_lock(chopstick + wantChops[0]);
        pthread_mutex_lock(chopstick + wantChops[1]);
        printf("%d is eatting with %d,%d chopstick\n", num, wantChops[0], wantChops[1]);
        pthread_mutex_unlock(chopstick + wantChops[0]);
        pthread_mutex_unlock(chopstick + wantChops[1]);
        printf("%d finish eatting\n", num);
        //冷静下
        think();
    }
}
int main() {
    srand((unsigned)time(NULL));
    pthread_t philosopher[PHILOSOPHER_NUM];

    // init mutex chopstick
    for(int i = 0; i < CHOPSTICK_NUM; i++) {
        pthread_mutex_init(chopstick + i, NULL);
    }

    // let's invite our philosophers to eat
    for(int i = 0;i < PHILOSOPHER_NUM; i++) {
        pthread_create(philosopher + i, NULL, (void*)&eat, (void*)i);
    }


    // wait our philosophers to finish
    for(int i = 0;i < PHILOSOPHER_NUM; i++)
        pthread_join(philosopher[i], NULL);
    
    // clean our chopstick
    for(int i = 0;i < CHOPSTICK_NUM; i++)
        pthread_mutex_destroy(chopstick + i);
    
    // time to finish our dinner
    return 0;
}
