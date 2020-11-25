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



//This function accepts a pointer to an image, its height, and its width then returns it's mean
int mean(image_ptr image, int height, int width){
  int n = height * width;
  int sum = 0;
  for(int i = 0; i < n; i++){
    sum += image[i];
  }
  return sum / n;
}

//This function accepts a pointer to an image, its height, and its width then returns it's standard deviation
int stdev(image_ptr image, int height, int width){
  int mu = mean(image, height, width);
  int n = height * width;
  int sum = 0;
  for(int i = 0; i < n; i++){
    sum += (image[i] - mu) * (image[i] - mu);
  }
  return sum / (n - 1);
}
