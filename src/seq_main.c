#include <math.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1

void apply_sobel_operator(uint8_t* input_image, int width, int height,
                          uint8_t* output_image) {
  // Sobel operator kernels
  int sobel_x_kernel[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

  int sobel_y_kernel[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

  int i, j, x, y, pixel;
  int gx, gy;
  int sobel_x, sobel_y;
  int magnitude;

  for (i = 1; i < height - 1; i++) {
    for (j = 1; j < width - 1; j++) {
      gx = 0;
      gy = 0;

      // Apply the Sobel operator in horizontal and vertical directions
      for (y = -1; y <= 1; y++) {
        for (x = -1; x <= 1; x++) {
          pixel = input_image[(i + y) * width + (j + x)];
          gx += pixel * sobel_x_kernel[y + 1][x + 1];
          gy += pixel * sobel_y_kernel[y + 1][x + 1];
        }
      }

      // Calculate the gradient magnitude
      magnitude = (int)sqrt(gx * gx + gy * gy);

      // Thresholding
      if (magnitude > 255) {
        magnitude = 255;
      }

      // Store the result in the output image
      output_image[i * width + j] = (uint8_t)magnitude;
    }
  }
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int width, height, bpp;

  // Reading the image in grey colors
  uint8_t* input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);

  printf("Width: %d  Height: %d \n", width, height);
  printf("Input: %s , Output: %s  \n", argv[1], argv[2]);

  // Allocate memory for the output image
  uint8_t* output_image =
      (uint8_t*)malloc(width * height * CHANNEL_NUM * sizeof(uint8_t));

  // start the timer
  double time1 = MPI_Wtime();

  apply_sobel_operator(input_image, width, height, output_image);

  double time2 = MPI_Wtime();
  printf("Elapsed time: %lf \n", time2 - time1);

  // Storing the image
  stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, output_image, 100);
  stbi_image_free(input_image);
  free(output_image);

  MPI_Finalize();
  return 0;
}
