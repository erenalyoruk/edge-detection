#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sobel.h"
#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1

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

  int width, height;

  unsigned char* input_image = NULL;
  unsigned char* output_image = NULL;

  if (rank == 0) {
    // Read the input image
    input_image = stbi_load(argv[1], &width, &height, NULL, CHANNEL_NUM);

    if (!input_image) {
      printf("Could not load image: %s", argv[1]);

      MPI_Finalize();
      return 1;
    }

    // Allocate memory for the output image
    output_image =
        (unsigned char*)malloc(width * height * sizeof(unsigned char));

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

  int end_row = (rank + 1) * chunk_size;
  if (rank == size - 1) {
    end_row = height;
  }

  // Allocate memory for the portion of the input image
  unsigned char* local_input_image = (unsigned char*)malloc(
      width * (end_row - start_row) * sizeof(unsigned char));

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
  unsigned char* local_output_image = (unsigned char*)malloc(
      width * (end_row - start_row) * sizeof(unsigned char));

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
  clock_t start = clock();

  // Perform edge detection on the local portion of the image
  apply_sobel_operator(local_input_image, width, end_row - start_row,
                       local_output_image);

  clock_t end = clock();
  printf("Rank: %d, Elapsed time: %lf\n", rank,
         (double)(end - start) / CLOCKS_PER_SEC);

  clock_t total = end - start;
  clock_t max;

  MPI_Reduce(&total, &max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Max Elapsed Time: %lf\n", (double)max / CLOCKS_PER_SEC);
  }

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
