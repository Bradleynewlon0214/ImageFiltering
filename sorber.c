#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "linkedlist.h"

typedef struct Work {
    int index;
    unsigned char *values;
} work_t;

typedef struct Node{
  work_t work;
  struct Node* next;
} node_t;




pthread_t pool[20];

void* thread_function(void *arg){

}

void array_push(unsigned char* arr, unsigned char value, int* len, int max){
  
  if(*len >= max){
      return;
  }

  arr[*len] = value;
  *len += 1;
}

int main(void){

    // for(int i = 0; i < 20; i++){
    //     pthread_create(&pool[i], NULL, thread_function, NULL);
    // }

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


    int q_len = 0;
    node_t *head = linkedlist_init(&q_len);
    printf("q_len: %d\n", q_len);

   
    enqueue(head, work, &q_len);
    enqueue(head, work_two, &q_len);
    enqueue(head, work_three, &q_len);

    printf("q_len: %d\n", q_len);

    printList(head);

    node_t *test;
    while((test = dequeue(head, &q_len)) != NULL){
        printf("%d\n", test->work.index);
        printf("q_len: %d\n", q_len);
    }

    printf("q_len: %d\n", q_len);

    printList(head);


}