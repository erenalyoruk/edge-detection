#include "sobel.h"

void apply_sobel_operator(unsigned char* input_image, int width, int height,
                          unsigned char* output_image) {
  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {
      int gx = input_image[(y - 1) * width + (x + 1)] -
               input_image[(y - 1) * width + (x - 1)] +
               2 * input_image[y * width + (x + 1)] -
               2 * input_image[y * width + (x - 1)] +
               input_image[(y + 1) * width + (x + 1)] -
               input_image[(y + 1) * width + (x - 1)];

      int gy = input_image[(y - 1) * width + (x - 1)] +
               2 * input_image[(y - 1) * width + x] +
               input_image[(y - 1) * width + (x + 1)] -
               input_image[(y + 1) * width + (x - 1)] -
               2 * input_image[(y + 1) * width + x] -
               input_image[(y + 1) * width + (x + 1)];

      int magnitude = (int)sqrt(gx * gx + gy * gy);

      magnitude = magnitude > 255 ? 255 : magnitude < 0 ? 0 : magnitude;

      output_image[y * width + x] = (unsigned char)magnitude;
    }
  }
}