#include "linkedlist.h"

enum Type {HEAD, DATA};

typedef struct Work {
    int index;
    unsigned char *values;
} work_t;

typedef struct Node{
  work_t work;
  struct Node* next;
  enum Type node_type;
} node_t;



node_t* linkedlist_init(int *len){
  node_t *head = malloc(sizeof(node_t));
  head->node_type = HEAD;
  *len = 0;
  return head;
}

void enqueue(node_t *head, work_t work, int *len){

  node_t *newNode = malloc(sizeof(node_t));

  newNode->work = work;
  newNode->node_type = DATA;

  node_t *temp = head->next;
  head->next = newNode;
  newNode->next = temp;

  // if(*len <= 0){
  //   head->next = newNode;
  // } else {
  //   node_t *it = head;
  //   for(int i = 0; i < *len; i++){
  //     it = it->next;
  //   }
  //   it->next = newNode;
  // }
  *len += 1;
}

node_t* dequeue(node_t *head, int *len){
  node_t *temp;

  if(head->next == NULL){
    return NULL;
  } else {
    temp = head->next;
    head->next = temp->next;
  }
  *len -= 1;
  return temp;
}

void printList(node_t *head){
  node_t *it;
  for(it = head; it != NULL; it = it->next){
    if(it->node_type != HEAD){
      printf(it->next != NULL ? "%d " : "%d\n", it->work.index);
    }
  }
}