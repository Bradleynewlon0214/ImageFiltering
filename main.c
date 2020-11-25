#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

#include <math.h>
#include <float.h>

//This is the only way I could get gcc to quit yelling at me.
#include "iplib2New.h"
#include "utils.h" 
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

int q_len = 0;
node_t *head;

pthread_t pool[20];
void* thread_function(void *arg){
  while(1){
    printf("Got work!\n");
    node_t* work_node = dequeue(head, &q_len);
    // if(work_node != NULL){
    //   printf("%d\n", work_node->work.index);
    // }
  }
}

void applyFilter(image_ptr, int[3][3], int, int);

void array_push(unsigned char* arr, unsigned char value, int* len, int max){
  
  if(*len >= max){
      return;
  }

  arr[*len] = value;
  *len += 1;
}

int main(int argc, char *argv[]){

  image_ptr imagePtr, combined, vertical, horizontal;

  int rows, cols;
  int type;

  printf("Reading input image... \n");

  imagePtr = read_pnm(argv[1], &rows, &cols, &type);

  int row_size = cols * 1;
  int total_size = (unsigned long) rows * row_size;

  combined = (image_ptr) malloc(total_size);
  
  printf("Image read successfully! \n");
  printf("rows=%d, cols=%d, type=%d \n", rows, cols, type);
  type = 5;


  dynamicCopyTo(1, imagePtr, total_size, combined);
  

  for(int i = 0; i < 20; i++){
    pthread_create(&pool[i], NULL, thread_function, NULL);
  }


  head = linkedlist_init(&q_len);
  for(int y = 1; y < rows - 1; y++){
    for(int x = 1; x < cols - 1; x++){

      unsigned char xi;
      int len = 0;
      unsigned char values[9];

      
      for(int p = -1; p <= 1; p++){
        for(int q = -1; q <=1; q++){
          xi = combined[(y + q) * cols + (x + p)];
          array_push(values, xi, &len, 9);
        }
      }


      work_t new_work;
      new_work.index = y * cols + x;
      new_work.values = values;

      enqueue(head, new_work, &q_len);

    }
  }
  write_pnm(combined, "binary.pgm", rows, cols, type);
}



//This function accepts a pointer to an image, a mask, and it's height and width
//The function loops through the image and on each pixel its computes it's edge response value given the mask.
//The pixel's value is then set to the absolute value of the edge response divided by 7. I explain why 7 in the function body.
void applyFilter(image_ptr image, int mask[3][3], int height, int width){

  int x, y, p, q; //loop control variables
  unsigned char e; //edge response value

  for(y = 1; y < height - 1; y++){
    for(x = 1; x < width - 1; x++){
      int index = y * width + x;

      e = 0; //Set edge response to 0 on each new pixel

      //kernel convolution
      for(p = -1; p <= 1; p++){
        for(q = -1; q <= 1; q++){
          unsigned char xi = image[(y + q) * width + (x + p)];
          e += mask[p + 1][q + 1] * xi;
        }
      }

      image[index] = abs(e / 7); //I was dividing this by 9 (size of mask) but, after experimentation, decided dividing by 7 produced a better result.

    }
  }
  
}