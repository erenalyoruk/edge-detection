#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sobel.h"
#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1

int main(int argc, char* argv[]) {
  int width = 0;
  int height = 0;

  // Reading the image in grey colors
  unsigned char* input_image =
      stbi_load(argv[1], &width, &height, NULL, CHANNEL_NUM);

  // Chech if image loaded
  if (!input_image) {
    printf("Could not load image: %s", argv[1]);
    return 1;
  }

  printf("Width: %d  Height: %d \n", width, height);
  printf("Input: %s , Output: %s  \n", argv[1], argv[2]);

  // Allocate memory for the output image
  unsigned char* output_image = (unsigned char*)malloc(
      width * height * CHANNEL_NUM * sizeof(unsigned char));

  // start the timer
  clock_t begin = clock();

  apply_sobel_operator(input_image, width, height, output_image);

  clock_t end = clock();
  printf("Elapsed time: %lf \n", (double)(end - begin) / CLOCKS_PER_SEC);

  stbi_image_free(input_image);

  // Storing the image
  stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, output_image, 100);
  free(output_image);

  return 0;
}
