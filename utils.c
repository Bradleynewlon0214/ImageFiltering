#include "utils.h"

void dynamicCopyTo(int num_items, image_ptr item, int len, ...){
  va_list args;

  va_start(args, len);
  for(int i = 0; i < num_items; i++){
    image_ptr test = va_arg(args, image_ptr);

    for(int j = 0; j < len; j++){
      *(test + j) = *(item + j);
    }

  }
  va_end(args);
}

void array_push(unsigned char* arr, unsigned char value, int* len, int max){
  if(*len >= max){
      return;
  }

  arr[*len] = value;
  *len += 1;
}


//This function accepts a pointer to an image, its height, and its width then returns it's mean
float mean(image_ptr image, int height, int width){
  float n = height * width;
  float sum = 0;
  for(int i = 0; i < n; i++){
    sum += (float)image[i];
  }
  return sum / n;
}

//This function accepts a pointer to an image, its height, and its width then returns it's standard deviation
float stdev(image_ptr image, int height, int width){
  float mu = mean(image, height, width);
  float n = height * width;
  float sum = 0;
  for(int i = 0; i < n; i++){
    sum += (float)((image[i] - mu) * (image[i] - mu));
  }
  return sqrt(sum / (n - 1));
}

//insertion sort for images
void insertion_sort(image_ptr image, int rows, int cols) { 
    int i, key, j;
    int n = rows * cols;

    for (i = 1; i < n; i++) { 
        key = image[i]; 
        j = i - 1; 
  
        /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */
        while (j >= 0 && image[j] > key) { 
            image[j + 1] = image[j]; 
            j = j - 1; 
        } 
        image[j + 1] = key; 
    } 
}

//get median of images
void median(image_ptr image, int rows, int cols, char filename[]){
  int n = rows * cols;
  printf("%s median: %u\n", filename, image[n / 2]);
}