#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>
#include <float.h>

//This is the only way I could get gcc to quit yelling at me.
#include "iplib2New.h" 

void dynamicCopyTo(int num_items, image_ptr item, int len, ...);
int mean(image_ptr, int, int);
int stdev(image_ptr, int, int);