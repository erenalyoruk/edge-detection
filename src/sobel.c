#include "sobel.h"

void apply_sobel_operator(unsigned char* input_image, int width, int height,
                          unsigned char* output_image) {
  int new_width = width + 2;
  int new_height = height + 2;

  unsigned char* padded_input_image =
      (unsigned char*)malloc(new_width * new_height * sizeof(unsigned char));

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      padded_input_image[(y + 1) * new_width + (x + 1)] =
          input_image[y * width + x];
    }
  }

  for (int x = 0; x < new_width; x++) {
    padded_input_image[x] = padded_input_image[new_width + x];
    padded_input_image[(new_height - 1) * new_width + x] =
        padded_input_image[(new_height - 2) * new_width + x];
  }

  for (int y = 0; y < new_height; y++) {
    padded_input_image[y * new_width] = padded_input_image[y * new_width + 1];
    padded_input_image[y * new_width + (new_width - 1)] =
        padded_input_image[y * new_width + (new_width - 2)];
  }

  for (int y = 1; y < new_height - 1; y++) {
    for (int x = 1; x < new_width - 1; x++) {
      int gx = padded_input_image[(y - 1) * new_width + (x + 1)] -
               padded_input_image[(y - 1) * new_width + (x - 1)] +
               2 * padded_input_image[y * new_width + (x + 1)] -
               2 * padded_input_image[y * new_width + (x - 1)] +
               padded_input_image[(y + 1) * new_width + (x + 1)] -
               padded_input_image[(y + 1) * new_width + (x - 1)];

      int gy = padded_input_image[(y - 1) * new_width + (x - 1)] +
               2 * padded_input_image[(y - 1) * new_width + x] +
               padded_input_image[(y - 1) * new_width + (x + 1)] -
               padded_input_image[(y + 1) * new_width + (x - 1)] -
               2 * padded_input_image[(y + 1) * new_width + x] -
               padded_input_image[(y + 1) * new_width + (x + 1)];

      int magnitude = (int)sqrt(gx * gx + gy * gy);

      magnitude = magnitude > 255 ? 255 : magnitude < 0 ? 0 : magnitude;

      output_image[(y - 1) * width + (x - 1)] = (unsigned char)magnitude;
    }
  }

  free(padded_input_image);
}
