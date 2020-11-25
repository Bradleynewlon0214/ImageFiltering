#include <stdio.h>
#include <stdlib.h>

typedef struct Work work_t;

typedef struct Node node_t;

node_t* linkedlist_init(int *len);

void enqueue(node_t *head, work_t work, int *len);

node_t* dequeue(node_t *head, int *len);

void printList(node_t *head);