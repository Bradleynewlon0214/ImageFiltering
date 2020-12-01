#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/types.h>

#include <math.h>
#include <float.h>


//This is the only way I could get gcc to quit yelling at me.
#include "iplib2New.h"
#include "utils.h"
#include "thpool.h"
#include "linkedlist.h"

void analysis(image_ptr image, char filename[]);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int rows, cols, type;
static const size_t num_threads = 20;
static int sobel_h[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static int sobel_v[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1}; 


void worker(int index, unsigned char *values, int *mask_one, int *mask_two, bool both){
  int i, j; //loop control variables
  unsigned char e = 0; //edge response variable

  if(both){
    unsigned char gx, gy = 0;
    for(i = 0, j = 9; i < 9, j >= 0; i++, j--){
      gx += mask_one[i] * values[j];
      gy += mask_two[i] * values[j];
    }
    e = sqrt((gy * gy) + (gx + gx));
  } else {
    for(i = 0, j = 9; i < 9, j >= 0; i++, j--){
      e += mask_one[i] * values[j]; //calculating edge response
    } 
  } 

  pthread_mutex_lock(&mutex);
  enqueue(index, e);          //putting edge response value and its corresponding index in a queue to be placed back in the image by the parent
  pthread_mutex_unlock(&mutex);

}

void process_image(image_ptr image_from, image_ptr image_target, bool both, int *mask_one, int *mask_two, int num_threads, char filename[]){
  tpool_t *tm;
  tm = tpool_create(num_threads);

  for(int y = 1; y < rows - 1; y++){
    for(int x = 1; x < cols - 1; x++){
      int index = y * cols + x;
      unsigned char *values = malloc(9 * sizeof(unsigned char));

      int len = 0;

      for(int p = -1; p <= 1; p++){
        for(int q = -1; q <= 1; q++){
          unsigned char xi = image_from[(y + q) * cols + (x + p)];
          array_push(values, xi, &len, 9);
        }
      }
      if(both){
        tpool_add_work(tm, worker, index, values, mask_one, mask_two, both);
      } else {
        tpool_add_work(tm, worker, index, values, mask_one, mask_one, both);
      }
      
    }
  }
  tpool_wait(tm);
  tpool_destroy(tm);


  node_t *head = getHead();
  node_t *it;
  for(it = head; it != NULL; it = it->next){
    int index = it->data.index;
    unsigned char e = it->data.e;

    image_target[index] = abs(e / 3);
  }
    
  clear_queue();

  write_pnm(image_target, filename, rows, cols, type);
}

void analysis(image_ptr image, char filename[]){
  float mu = mean(image, rows, cols);
  float s = stdev(image, rows, cols);

  printf("%s mean: %f0.2 stdev: %f0.2\n", filename, mu, s);
}

void bin(image_ptr image, char filename[]){
    float mu = mean(image, rows, cols);
    float s = stdev(image, rows, cols);
    float t = (mu + s);
    
    for(int y = 0; y < rows; y++){
      for(int x = 0; x < cols; x++){
        int index = y * cols + x;
        float exy = image[index];
        if(exy > t){
          image[index] = 255;
        } else{
          image[index] = 0;
        }
      }
    }
    write_pnm(image, filename, rows, cols, type);
}

int main(int argc, char *argv[]){

  image_ptr imagePtr, combined, vertical, horizontal;

  printf("Reading input image... \n");

  imagePtr = read_pnm(argv[1], &rows, &cols, &type);

  int row_size = cols * 1;
  int total_size = (unsigned long) rows * row_size;
  
  vertical = (image_ptr) malloc(total_size);
  horizontal = (image_ptr) malloc(total_size);
  combined = (image_ptr) malloc(total_size);
  
  printf("Image read successfully! \n");
  printf("rows=%d, cols=%d, type=%d \n", rows, cols, type);
  type = 5;


  

  dynamicCopyTo(3, imagePtr, total_size, vertical, horizontal, combined);


  process_image(imagePtr, vertical, false, sobel_v, sobel_v, num_threads, "vertical.pgm");
  process_image(imagePtr, horizontal, false, sobel_h, sobel_h, num_threads, "horizontal.pgm");
  process_image(imagePtr, combined, true, sobel_h, sobel_v, num_threads, "combined.pgm");
  bin(combined, "binary.pgm");


  analysis(vertical, "vertical.pgm");
  // insertion_sort(vertical, rows, cols);
  // median(vertical, rows, cols, "vertical.pgm");

  analysis(horizontal, "horizontal.pgm");
  // insertion_sort(horizontal, rows, cols);
  // median(horizontal, rows, cols, "horizontal.pgm");

  analysis(combined, "combined.pgm");
  // insertion_sort(combined, rows, cols);
  // median(combined, rows, cols, "combined.pgm");

  analysis(imagePtr, argv[1]);
  // insertion_sort(imagePtr, rows, cols);
  // median(imagePtr, rows, cols, argv[1]);

}