#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

//This is the only way I could get gcc to quit yelling at me.
#include "iplib2New.h" 
// #include "iplib2New.c"


#define rr 200
#define cc 225

int ROWS, COLS, TYPE;

void applyFilter(image_ptr, int[3][3], int, int);
void applyBothFilter(image_ptr, int[3][3], int[3][3], int, int);
int mean(image_ptr, int, int);
int stdev(image_ptr, int, int);
void bin(image_ptr, int, int);


int main(int argc, char *argv[]){

  image_ptr imagePtr, combined, vertical, horizontal;
  unsigned char combinedImage[rr][cc], verticalImage[rr][cc], horizontalImage[rr][cc];  /* space for output image */

  int sobel_h[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}; //Horizontal
  int sobel_v[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}; //Vertical
    
  int rows, cols;
  int type;

  printf("Reading input image... \n");

  imagePtr = read_pnm(argv[1], &rows, &cols, &type);

  printf("Image read successfully! \n");
  printf("rows=%d, cols=%d, type=%d \n", rows, cols, type);
  type = 5;


  //Processing Horizontal Mask
  for (int i = 0; i < rr; i++){
    for (int j = 0; j < cc; j++){
      horizontalImage[i][j] = imagePtr[i*cols + j];
    }
  }
  horizontal = (image_ptr)horizontalImage;
  printf("\nStatistics for Original Image: Mean: %d, Standard Deviation: %d\n", mean(horizontal, rr, cc), stdev(horizontal, rr, cc));

  applyFilter(horizontal, sobel_h, rr, cc);

  printf("\nNow saving horizontal mask image... \n");
  printf("Statistics for Horizontal Image: Mean: %d, Standard Deviation: %d\n", mean(horizontal, rr, cc), stdev(horizontal, rr, cc));

  write_pnm(horizontal, "horizontal.pgm", rr, cc, type);
  printf("Horizontal image saved!\n"); 
  //End Processing Horizontal



  //Processing Vertical Mask
  for (int i = 0; i < rr; i++){
    for (int j = 0; j < cc; j++){
      verticalImage[i][j] = imagePtr[i*cols + j];
    }
  }
  vertical = (image_ptr)verticalImage;
  applyFilter(vertical, sobel_v, rr, cc);
  printf("\nNow saving vertical mask image... \n");
  printf("Statistics for Vertical Image: Mean: %d, Standard Deviation: %d\n", mean(vertical, rr, cc), stdev(vertical, rr, cc));

  write_pnm(vertical, "vertical.pgm", rr, cc, type);
  printf("Vertical image saved!\n"); 
  //End Processing Vertical

  


  //Processing Combined Mask
  for (int i = 0; i < rr; i++){
    for (int j = 0; j < cc; j++){
      combinedImage[i][j] = imagePtr[i*cols + j];
    }
  }
  combined = (image_ptr)combinedImage;
  applyBothFilter(combined, sobel_h, sobel_v, rr, cc);
  printf("\nNow saving combined mask image... \n");
  printf("Statistics for Combined Image: Mean: %d, Standard Deviation: %d\n", mean(combined, rr, cc), stdev(combined, rr, cc));

  write_pnm(combined, "combined.pgm", rr, cc, type);
  printf("Combined image saved!\n"); 
  //End Processing Combined Mask
  


  //Processing Binary Image
  bin(combined, rr, cc);
  printf("\nNow saving binary image... \n");
  printf("Statistics for Binary Image: Mean: %d, Standard Deviation: %d\n", mean(combined, rr, cc), stdev(combined, rr, cc));

  write_pnm(combined, "binary.pgm", rr, cc, type);
  printf("Binary image saved!\n");
  //End Processing Binary Image
  
  return 0;
}


//This function performs the binarization process for an edge response.
//This function accepts an image pointer, its height, and it's width. First, the functio computes the images mean and standard deviation then loops through the image
//If the edge response value is greater than the threshold value (int t = mu + k * s), it's value is set to 1, else 0.
void bin(image_ptr image, int height, int width){
    int mu = mean(image, height, width);
    int s = stdev(image, height, width);
    int k = 1;
    int t = mu + k * s;
    for(int y = 0; y < height; y++){
      for(int x = 0; x < width; x++){
        int index = y * width + x;
        int exy = image[index];
        if(exy > t){
          image[index] = 255;
        } else{
          image[index] = 0;
        }
      }
    }
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

//This function accepts a pointer to an image, a horizontal mask, a vertical mask, and the images height and width
//The function loops through the image and on each pixel performs a kernel convolution computing the edge response values for each mask
//then the pixel's value is set to the value of 9 divided by the square root of the summation of the two edge responses squared.
void applyBothFilter(image_ptr image, int maskHorizontal[3][3], int maskVertical[3][3], int height, int width){
  int x, y, p, q; //loop control variables
  unsigned char gx, gy; //edge response value for horizontal (gx) and response value for vertical (gy)

  for(y = 1; y < height - 1; y++){
    for(x = 1; x < width - 1; x++){
      int index = y * width + x;

      //Set the edge responses to 0 for each new pixel
      gx = 0;
      gy = 0;

      //kernel convolution
      for(p = -1; p <= 1; p++){
        for(q = -1; q <= 1; q++){
          unsigned char xi = image[(y + q) * width + (x + p)];
          gx += maskHorizontal[p + 1][q + 1] * xi;
          gy += maskVertical[p + 1][q + 1] * xi;
        }
      }

      image[index] = sqrt((gy * gx) + (gx * gx)) / 9; //Nine is the size of the mask
    }
  }
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

