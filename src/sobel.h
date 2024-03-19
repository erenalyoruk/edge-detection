#pragma once

#include <math.h>
#include <stdint.h>

void apply_sobel_operator(uint8_t* input_image, int width, int height,
                          uint8_t* output_image);