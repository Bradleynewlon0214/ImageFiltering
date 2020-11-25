#include <stdio.h>
#include <stdlib.h>

/* file types*/
#define PBM 4
#define PGM 5
#define PPM 6

typedef unsigned char *image_ptr;

image_ptr read_pnm(char *filename, int *rows, int *cols, int *type);
int getnum(FILE *fp);
void write_pnm(image_ptr ptr, char *filename, int rows, int cols, int type);