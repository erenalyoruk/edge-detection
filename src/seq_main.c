#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sobel.h"
#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1

int main(int argc, char* argv[]) {
  int width, height, bpp;

  // Reading the image in grey colors
  uint8_t* input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);

  printf("Width: %d  Height: %d \n", width, height);
  printf("Input: %s , Output: %s  \n", argv[1], argv[2]);

  // Allocate memory for the output image
  uint8_t* output_image =
      (uint8_t*)malloc(width * height * CHANNEL_NUM * sizeof(uint8_t));

  // start the timer
  clock_t begin = clock();

  apply_sobel_operator(input_image, width, height, output_image);

  clock_t end = clock();
  printf("Elapsed time: %lf \n", (double)(end - begin) / CLOCKS_PER_SEC);

  // Storing the image
  stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, output_image, 100);
  stbi_image_free(input_image);
  free(output_image);

  return 0;
}
