#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct threadpool pool_t;

pool_t pool_init(int num_threads);

int add_work(pool_t, void (*function_p)(void*), void* arg_p);

void pool_wait(pool_t);

void pool_destroy(pool_t);
