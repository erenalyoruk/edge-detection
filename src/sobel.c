#include "sobel.h"

void apply_sobel_operator(uint8_t* input_image, int width, int height,
                          uint8_t* output_image) {
  // Sobel operator kernels
  int sobel_x_kernel[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

  int sobel_y_kernel[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

  int i, j, x, y, pixel;
  int gx, gy;
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