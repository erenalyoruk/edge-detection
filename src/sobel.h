#pragma once

#include <math.h>
#include <stdint.h>
#include <string.h>

void apply_sobel_operator(unsigned char* input_image, int width, int height,
                          unsigned char* output_image);