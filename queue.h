#include <stdio.h>
#include <stdlib.h>


typedef struct Data{
  int index;
  unsigned char e;
} data_t;

typedef struct Node{
  data_t data;
  struct Node* next;
} node_t;

node_t* getHead();
void enqueue(int index, unsigned char e);
void clear_queue();
void printList();