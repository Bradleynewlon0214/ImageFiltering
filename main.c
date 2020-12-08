#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "iplib2New.h"

#include "utils.h"
#include "thpool.h"
#include "queue.h"

//Variables for main
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex lock for adding to queue in threads

int rows, cols, type;
static int sobel_h[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static int sobel_v[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};


//===========================================================================================================================================================================
//============================================================== FUNCTION PROTOTYPES FOR MAIN ===============================================================================
//============================================================== Main functions begin on Line 86 ============================================================================
//===========================================================================================================================================================================

void worker(int index, unsigned char *values, int *mask_one, int *mask_two, bool both);
void process_image(image_ptr image_from, image_ptr image_target, bool both, int *mask_one, int *mask_two, int num_threads, char filename[]);
void bin(image_ptr image, char filename[]);



//===========================================================================================================================================================================
//============================================================== FUNCTION DEFINITIONS FOR MAIN ==============================================================================
//============================================================== Main begins on Line 200 ====================================================================================
//===========================================================================================================================================================================


/*
* This funcion is the function that each thread uses to calculate a subblocks edge response values. It accepts an index, an array of the sublock's values,
* two image masks, and a boolean that determines if the function will calculate the edge response using either one or both image masks. The function will then take the 
* mutex lock and add the edge response value and its index to the queue.
* Although the function requires two images masks only one will get used if the boolean is false so in that case you can just pass in the same mask twice.
*/
void worker(int index, unsigned char *values, int *mask_one, int *mask_two, bool both){
  int i, j; //loop control variables
  unsigned char e = 0; //edge response variable

  if(both){
    unsigned char gx, gy = 0;
    for(i = 0, j = 9; i < 9, j >= 0; i++, j--){
      gx += mask_one[i] * values[j]; //calculating horizontal edge response
      gy += mask_two[i] * values[j]; //calculatting vertical edge response
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

/*
* This function accepts an image to calculate edge response values for, a new location to place the edger response values, a boolean that indicates whether or not
* this is the combined edge response, two image masks, the number of threads to be created in the thread pool, and a filename to save the output image.
* The function will loop thougth the whole image and allot sublocks and their respective indices and push them to a thread pool.
* When the threads are finished working, the function will then take the edge response values and place them at their respective index in the output image then save the image.
* Although the function requires two images masks only one will get used if the boolean is false so in that case you can just pass in the same mask twice.
*/
void process_image(image_ptr image_from, image_ptr image_target, bool both, int *mask_one, int *mask_two, int num_threads, char filename[]){
  tpool_t *tm;
  tm = tpool_create(num_threads); //create thread pool

  //loop through image, allocate subblock of values and its respective index, and add to work queue
  for(int y = 1; y < rows - 1; y++){
    for(int x = 1; x < cols - 1; x++){
      int index = y * cols + x; //current index
      unsigned char *values = malloc(9 * sizeof(unsigned char)); //allocating array where subblock's values will be stored

      int len = 0; //length of values array. will be used in array_push

      for(int p = -1; p <= 1; p++){
        for(int q = -1; q <= 1; q++){
          unsigned char xi = image_from[(y + q) * cols + (x + p)]; //getting current subblock value
          array_push(values, xi, &len, 9); //pushing value to values array
        }
      }

      //push work to work queue. if we are doing both edge responses both will be true
      if(both){
        tpool_add_work(tm, worker, index, values, mask_one, mask_two, both);
      } else {
        tpool_add_work(tm, worker, index, values, mask_one, mask_one, both);
      }
      
    }
  }
  //wait for threads to finish working
  tpool_wait(tm);
  tpool_destroy(tm);

  //get finished edge response values, their respective indicies, and place them in the edge response image 
  node_t *head = getHead();
  node_t *it;
  for(it = head; it != NULL; it = it->next){
    int index = it->data.index;
    unsigned char e = it->data.e;

    image_target[index] = abs(e / 3);
  }
  //clear queue for next image  
  clear_queue();

  //write image out
  write_pnm(image_target, filename, rows, cols, type);
}

/*
* This function performs the binarization process for an edge response.
* This function accepts an image pointer, its height, and it's width. First, the functio computes the images mean and standard deviation then loops through the image
* If the edge response value is greater than the threshold value (int t = mu + k * s), it's value is set to 1, else 0.
*/
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

  char usage[50];
  sprintf(usage, "%s <pgm file> <num_threads>", argv[0]);
  if(argc < 3){  
    printf("%s\n", usage);
    exit(1);
  }
  int num_threads = (int)atof(argv[2]);

  imagePtr = read_pnm(argv[1], &rows, &cols, &type);

  int row_size = cols * 1;
  int total_size = (unsigned long) rows * row_size;
  
  vertical = (image_ptr) malloc(total_size);
  horizontal = (image_ptr) malloc(total_size);
  combined = (image_ptr) malloc(total_size);
  printf("Image read successfully! \n\n");
  type = 5;

  dynamicCopyTo(3, imagePtr, total_size, vertical, horizontal, combined);

  pid_t pid, wpid;
  int status = 0; 

  pid = fork(); //fork

  /*
  * child process does vertical and horizontal edge responses
  * parent process does combinded edge response and binary image
  */
  if(pid == 0){
    process_image(imagePtr, vertical, false, sobel_v, sobel_v, num_threads, "vertical.pgm");
    process_image(imagePtr, horizontal, false, sobel_h, sobel_h, num_threads, "horizontal.pgm");
  } else {
    process_image(imagePtr, combined, true, sobel_h, sobel_v, num_threads, "combined.pgm");
    bin(combined, "binary.pgm");
  }

  /*
  * child does vertical and horizontal analysis
  * parent does combined and original image analysis
  */
//   if(pid == 0){
//     analysis(vertical, rows, cols,  "vertical.pgm");
//     insertion_sort(vertical, rows, cols);
//     median(vertical, rows, cols, "vertical.pgm");

//     analysis(horizontal, rows, cols, "horizontal.pgm");
//     insertion_sort(horizontal, rows, cols);
//     median(horizontal, rows, cols, "horizontal.pgm");
//     exit(0);

//   } else {
//     analysis(combined, rows, cols, "combined.pgm");
//     insertion_sort(combined, rows, cols);
//     median(combined, rows, cols, "combined.pgm");

//     analysis(imagePtr, rows, cols,  argv[1]);
//     insertion_sort(imagePtr, rows, cols);
//     median(imagePtr, rows, cols, argv[1]);
//   }

  while((wpid = wait(&status)) > 0); //wait for child to finish

  printf("Finished!\n");
}
