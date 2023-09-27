#ifndef LIBRARY_H
#define LIBRARY_H

#include "cbmp.h"

#define PACKED_WIDTH 119

// Prototypes
void grayscale_and_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[PACKED_WIDTH][BMP_HEIGTH]);
void single_to_multi_channel(unsigned char input_image[PACKED_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int erode(unsigned char input_image[PACKED_WIDTH][BMP_HEIGTH], unsigned char output_image[PACKED_WIDTH][BMP_HEIGTH], int iteration);
void detect(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], unsigned char draw_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned short *number_of_cells);

#endif // LIBRARY_H
