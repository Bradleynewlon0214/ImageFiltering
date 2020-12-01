#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>
#include <float.h>

//This is the only way I could get gcc to quit yelling at me.
#include "iplib2New.h" 

void dynamicCopyTo(int num_items, image_ptr item, int len, ...);
void array_push(unsigned char* arr, unsigned char value, int* len, int max);
float mean(image_ptr, int, int);
float stdev(image_ptr, int, int);
void median(image_ptr image, int rows, int cols, char filename[]);
void insertion_sort(image_ptr image, int rows, int cols);
