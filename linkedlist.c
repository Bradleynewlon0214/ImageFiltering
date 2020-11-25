#include "linkedlist.h"

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(work_t* work, int *len){

  node_t *newNode = malloc(sizeof(node_t));

  newNode->work = work;
  newNode->next = NULL;

  if(tail == NULL){
    head = newNode;
  } else{
    tail->next = newNode;
  }

  tail = newNode;

  *len += 1;
}

work_t* dequeue(int *len){
  if(head == NULL){
    return NULL;
  } else {
    work_t* result = head->work;
    node_t *temp = head;
    head = head->next;
    if(head == NULL) { tail == NULL; }
    free(temp);
    *len -= 1;
    return result;
  }
}

void printList(){
  node_t *it;
  for(it = head; it != NULL; it = it->next){
    printf(it->next != NULL ? "%d " : "%d\n", it->work->index);
  }
}