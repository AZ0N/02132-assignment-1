#ifndef LIBRARY_H 
#define LIBRARY_H

#include "cbmp.h"

#define THRESHOLD 90
#define CROSS_RADIUS 8
#define DETECTION_HALF_RADIUS 6
#define MIN_WHITE_PIXELS 2

// Prototypes
void grayscale_and_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
void single_to_multi_channel(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int iteration);
int match_straight(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
int match_diagonal(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
void detect(unsigned char image[BMP_WIDTH][BMP_HEIGTH], unsigned char draw_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned short *number_of_cells);
void draw_cross(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);

#endif // LIBRARY_H
