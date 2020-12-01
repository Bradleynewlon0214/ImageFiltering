#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <stdbool.h>
#include <stddef.h>
#include "iplib2New.h"

struct tpool;
typedef struct tpool tpool_t;

typedef void (*thread_func_t)(int index, unsigned char *values, int *mask_one, int* mask_two, bool both);

tpool_t *tpool_create(size_t num);
void tpool_destroy(tpool_t *tm);

bool tpool_add_work(tpool_t *tm, thread_func_t func, int index, unsigned char *values, int *mask_one, int *mask_two, bool both);
void tpool_wait(tpool_t *tm);

#endif /* __TPOOL_H__ */