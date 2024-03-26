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

  if (argc != 3) {
    if (rank == 0) {
      fprintf(stderr, "Usage: %s <input_image> <output_image>\n", argv[0]);
    }

    MPI_Finalize();
    return 1;
  }

  int width, height;

  unsigned char* input_image = NULL;
  if (rank == 0) {
    input_image = stbi_load(argv[1], &width, &height, NULL, CHANNEL_NUM);

    if (input_image == NULL) {
      fprintf(stderr, "Failed to load image: %s\n", argv[1]);

      MPI_Finalize();
      return 1;
    }
  }

  MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int* send_counts = NULL;
  int* send_displs = NULL;
  if (rank == 0) {
    send_counts = (int*)malloc(size * sizeof(int));
    send_displs = (int*)malloc(size * sizeof(int));

    int offset = 0;
    int remainder = height % size;

    for (int i = 0; i < size; ++i) {
      send_counts[i] = (height / size) * width;
      if (remainder > 0) {
        send_counts[i] += width;
        --remainder;
      }

      send_displs[i] = offset;
      offset += send_counts[i];
    }
  }

  int local_height = height / size;
  int remainder = height % size;
  if (remainder > rank) {
    ++local_height;
  }

  unsigned char* local_image =
      (unsigned char*)malloc(local_height * width * sizeof(unsigned char));
  if (local_image == NULL) {
    fprintf(stderr, "Failed to allocate local image in %d!\n", rank);

    MPI_Finalize();
    return 1;
  }

  MPI_Scatterv(input_image, send_counts, send_displs, MPI_UNSIGNED_CHAR,
               local_image, local_height * width, MPI_UNSIGNED_CHAR, 0,
               MPI_COMM_WORLD);

  unsigned char* local_output =
      (unsigned char*)malloc(local_height * width * sizeof(unsigned char));

  clock_t start = clock();
  apply_sobel_operator(local_image, width, local_height, local_output);
  clock_t end = clock();

  double time = ((double)end - start) / CLOCKS_PER_SEC;
  double max;

  MPI_Reduce(&time, &max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    fprintf(stdout, "Elapsed time: %lf\n", max);
  }

  unsigned char* output = NULL;
  if (rank == 0) {
    output = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (output == NULL) {
      fprintf(stderr, "Failed to allcoate output image!\n");

      MPI_Finalize();
      return 1;
    }
  }

  MPI_Gatherv(local_output, local_height * width, MPI_UNSIGNED_CHAR, output,
              send_counts, send_displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, output, 100);

    free(output);
    free(input_image);
  }

  free(local_image);
  free(local_output);

  MPI_Finalize();
  return 0;
}
