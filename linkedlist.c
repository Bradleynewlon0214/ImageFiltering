#include "linkedlist.h"

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(int index, unsigned char e){

  node_t *newNode = malloc(sizeof(node_t));
  data_t data = {index, e}; 

  newNode->data = data;
  newNode->next = NULL;

  if(tail == NULL){
    head = newNode;
  } else{
    tail->next = newNode;
  }

  tail = newNode;
}

data_t* dequeue(){
  if(head == NULL){
    return NULL;
  } else {
    data_t *result = &head->data;
    node_t *temp = head;
    head = head->next;
    if(head == NULL) { tail == NULL; }
    return result;
  }
}

void clear_queue(){
  node_t *it = head;
  while(it != NULL){
    node_t *prev = it;
    it = it->next;
    free(prev);
  }

  head = NULL;
  tail = NULL;
  
}

node_t* getHead(){
  return head;
}

void printList(){
  node_t *it;
  for(it = head; it != NULL; it = it->next){

    printf("Index: %d Edge Response: %u \n", it->data.index, it->data.e);

    // printf(it->next != NULL ? "%ul " : "%ul\n", it->data.e);
  }
}