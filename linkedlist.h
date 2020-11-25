#include <stdio.h>
#include <stdlib.h>


typedef struct Work {
    int index;
    unsigned char *values;
} work_t;

typedef struct Node{
  work_t *work;
  struct Node* next;
} node_t;


void enqueue(work_t* work, int *len);

work_t* dequeue(int *len);

void printList();