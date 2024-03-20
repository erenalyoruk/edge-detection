#include "sobel.h"

static const int SOBEL_KERNEL_X[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
static const int SOBEL_KERNEL_Y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

void apply_sobel_operator(unsigned char* input_image, int width, int height,
                          unsigned char* output_image) {
  double gradient = 0.0;

  memset(output_image, 0, width * height);

  for (int i = 1; i < height - 1; i++) {
    for (int j = 1; j < width - 1; j++) {
      int gx = 0;
      int gy = 0;

      // Apply the Sobel operator in horizontal and vertical directions
      for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
          int pixel = input_image[(i + y) * width + (j + x)];
          gx += pixel * SOBEL_KERNEL_X[y + 1][x + 1];
          gy += pixel * SOBEL_KERNEL_Y[y + 1][x + 1];
        }
      }

      // Calculate the gradient magnitude
      gradient = sqrt((double)(gx * gx + gy * gy));

      // Thresholding
      if (gradient > 255) {
        gradient = 255;
      } else if (gradient < 0) {
        gradient = 0;
      }

      // Store the result in the output image
      output_image[i * width + j] = (unsigned char)gradient;
    }
  }
}