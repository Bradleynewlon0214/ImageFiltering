#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "linkedlist.h"




int main(int argc, char *argv[]){

    enqueue(9, 9);
    enqueue(9, 9);
    enqueue(9, 9);
    enqueue(9, 9);
    enqueue(9, 9);
    enqueue(9, 9);

    printList();

    clear_queue();


    enqueue(3, 3);

    printList();

    return 0;
}