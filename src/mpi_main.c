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

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 3) {
    if (rank == 0) {
      printf("Usage: %s <input_image> <output_image>\n", argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  int width, height, bpp;
  uint8_t* input_image = NULL;
  uint8_t* output_image = NULL;

  if (rank == 0) {
    // Read the input image
    input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
    if (!input_image) {
      printf("Error loading the input image.\n");
      MPI_Finalize();
      return 1;
    }

    // Allocate memory for the output image
    output_image = (uint8_t*)malloc(width * height * sizeof(uint8_t));
    if (!output_image) {
      printf("Memory allocation failed.\n");
      stbi_image_free(input_image);
      MPI_Finalize();
      return 1;
    }

    printf("Width: %d  Height: %d \n", width, height);
    printf("Input: %s , Output: %s  \n", argv[1], argv[2]);
  }

  // Broadcast image dimensions
  MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Calculate the portion of the image to be processed by each process
  int chunk_size = height / size;
  int start_row = rank * chunk_size;
  int end_row = (rank == size - 1) ? height : (rank + 1) * chunk_size;

  // Allocate memory for the portion of the input image
  uint8_t* local_input_image =
      (uint8_t*)malloc(width * (end_row - start_row) * sizeof(uint8_t));
  if (!local_input_image) {
    printf("Memory allocation failed.\n");
    if (rank == 0) {
      stbi_image_free(input_image);
      free(output_image);
    }
    MPI_Finalize();
    return 1;
  }

  // Scatter the input image data among processes
  MPI_Scatter(input_image, width * chunk_size, MPI_UNSIGNED_CHAR,
              local_input_image, width * chunk_size, MPI_UNSIGNED_CHAR, 0,
              MPI_COMM_WORLD);

  // Allocate memory for the portion of the output image
  uint8_t* local_output_image =
      (uint8_t*)malloc(width * (end_row - start_row) * sizeof(uint8_t));
  if (!local_output_image) {
    printf("Memory allocation failed.\n");
    if (rank == 0) {
      stbi_image_free(input_image);
      free(output_image);
    }
    free(local_input_image);
    MPI_Finalize();
    return 1;
  }

  // Start the timer
  double time1 = MPI_Wtime();

  // Perform edge detection on the local portion of the image
  apply_sobel_operator(local_input_image, width, end_row - start_row,
                       local_output_image);

  double time2 = MPI_Wtime();
  printf("Elapsed time: %lf \n", time2 - time1);

  // Gather the results from all processes to the root process
  MPI_Gather(local_output_image, width * chunk_size, MPI_UNSIGNED_CHAR,
             output_image, width * chunk_size, MPI_UNSIGNED_CHAR, 0,
             MPI_COMM_WORLD);

  // Write the output image
  if (rank == 0) {
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, output_image, 100);

    // Free memory
    stbi_image_free(input_image);
    free(output_image);
  }

  // Free memory
  free(local_input_image);
  free(local_output_image);

  MPI_Finalize();
  return 0;
}
