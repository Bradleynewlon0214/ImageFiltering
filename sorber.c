#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "linkedlist.h"



pthread_t pool[20];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int q_len;

void* thread_function(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        dequeue(&q_len);
        pthread_mutex_unlock(&mutex);
  }
}


int main(void){

    for(int i = 0; i < 20; i++){
        pthread_create(&pool[i], NULL, thread_function, NULL);
    }

    unsigned char values[9] = {0};

    work_t work = {
        15,
        values,
    };
    work_t work_two = {
        16,
        values,
    };

    work_t work_three = {
        17,
        values,
    };


    q_len = 0;

    enqueue(&work, &q_len);

    enqueue(&work_two, &q_len);

    enqueue(&work_three, &q_len);


    // printList();


    // work_t *test;
    // while((test = dequeue(&q_len)) != NULL){
    //     printf("%d\n", test->index);
    //     printf("q_len: %d\n", q_len);
    // }

}