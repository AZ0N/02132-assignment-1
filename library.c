#include <stdio.h>
#include "library.h"

// Controls whether the centers of cells are printed to standard out
#define PRINT_CENTERS

#define THRESHOLD 90
#define CROSS_RADIUS 7
#define DETECTION_HALF_RADIUS 7
#define MIN_WHITE_PIXELS 2
#define MAX_WHITE_IN_EXCLUSION 1

// Computing THRESHOLD * 3/4 so the average in grayscale_and_threshold can be computed division by 4 (using bitshift, << 2).
static const unsigned char THRESHOLD_SHIFTED = THRESHOLD * 3 / 4;

unsigned char get_pixel(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (image[x >> 3][y] >> (7 - (x & 0b111))) & 1;
}

void clear_pixel(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    image[x >> 3][y] &= ~(1 << (7 - (x & 0b111)));
}

void set_pixel(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    image[x >> 3][y] |= 1 << (7 - (x & 0b111));
}

// Prototypes
void draw_cross(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int in_bounds(int x, int y);
int check_frame(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y, int size);

// Convert input[][] to grayscale and apply binary threshold and save in output[][]
void grayscale_and_threshold(unsigned char input[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output[PACKED_WIDTH][BMP_HEIGTH])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            // Calculate and check shifted average
            unsigned char average_shifted = (input[x][y][0] + input[x][y][1] + input[x][y][2]) >> 2;
            // If shifted average is over shifted threshold, set the corresponding bit to 1, else 0
            (average_shifted > THRESHOLD_SHIFTED) ? set_pixel(output, x, y) : clear_pixel(output, x, y);
        }
    }
}

// Convert bit-packed image to BMP image format
void single_to_multi_channel(unsigned char input_image[PACKED_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            for (int c = 0; c < BMP_CHANNELS; c++)
            {
                output_image[x][y][c] = get_pixel(input_image, x, y) ? 255 : 0;
            }
        }
    }
}

// Detect cells in image[][]. Writes crosses of found cells to draw_image. Increments *number_of_cells
void detect(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], unsigned char draw_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned short *number_of_cells)
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            int white_in_exclusion_inner = check_frame(image, x, y, DETECTION_HALF_RADIUS + 1);
            int white_in_exclusion_outer = check_frame(image, x, y, DETECTION_HALF_RADIUS + 2);

            // If we found a white pixel in the exclusion frame, move the detection windows 1 over
            if (white_in_exclusion_inner + white_in_exclusion_outer > MAX_WHITE_IN_EXCLUSION)
            {
                continue;
            }
            // Exclusion frame is all black (or empty), check if any white pixels inside
            int white_pixels_found = 0;

            for (int dx = -DETECTION_HALF_RADIUS; dx <= DETECTION_HALF_RADIUS; dx++)
            {
                for (int dy = -DETECTION_HALF_RADIUS; dy <= DETECTION_HALF_RADIUS; dy++)
                {
                    if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH && get_pixel(image, x + dx, y + dy))
                    {
                        white_pixels_found += 1;
                        // If we found enough white pixels break, and go to clearing the frame
                        if (white_pixels_found > MIN_WHITE_PIXELS)
                        {
                            break;
                        }
                    }
                }
            }
            // If we found enough white pixels, we detected a cell
            if (white_pixels_found > MIN_WHITE_PIXELS)
            {
                // Clear frame
                for (int dx = -DETECTION_HALF_RADIUS; dx <= DETECTION_HALF_RADIUS; dx++)
                {
                    for (int dy = -DETECTION_HALF_RADIUS; dy <= DETECTION_HALF_RADIUS; dy++)
                    {
                        if (in_bounds(x + dx, y + dy) && get_pixel(image, x + dx, y + dy))
                        {
                            clear_pixel(image, x + dx, y + dy);
                        }
                    }
                }
#ifdef PRINT_CENTERS
                printf("(%d, %d)\n", x, y);
#endif
                *number_of_cells += 1;
                draw_cross(x, y, draw_image);
            }
            // If the entire capture area was empty (all black pixels), move the capture array half the width further
            else if (white_pixels_found == 0)
            {
                y += DETECTION_HALF_RADIUS + 1;
            }
        }
    }
}

// Draw a red cross on image with center at (x,y)
void draw_cross(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
    // Draw horizontal line
    for (int dx = -CROSS_RADIUS; dx <= CROSS_RADIUS; dx++)
    {
        for (int dy = -1; dy <= 2; dy++)
        {
            if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH)
            {
                image[x + dx][y + dy][0] = 255;
                image[x + dx][y + dy][1] = image[x + dx][y + dy][2] = 0;
            }
        }
    }
    // Draw vertical line
    for (int dy = -CROSS_RADIUS; dy <= CROSS_RADIUS; dy++)
    {
        for (int dx = -1; dx <= 2; dx++)
        {
            if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH)
            {
                image[x + dx][y + dy][0] = 255;
                image[x + dx][y + dy][1] = image[x + dx][y + dy][2] = 0;
            }
        }
    }
}

// Returns 1 if a given (x, y) coordinate is inside the bounds, else 0
int in_bounds(int x, int y)
{
    return 0 <= x && x < BMP_WIDTH && 0 <= y && y < BMP_HEIGTH;
}

// Returns the number of white pixels found in a frame around (x,y) in image[][], going size out to each side (2*side + 1 width)
int check_frame(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y, int size)
{
    int white_in_frame = 0;
    // First check exclusion frame
    for (int offset = -size; offset <= size; offset++)
    {
        if (in_bounds(x + offset, y - size) && get_pixel(image, x + offset, y - size) || // Top row
            in_bounds(x + offset, y + size) && get_pixel(image, x + offset, y + size) || // Bottom row
            in_bounds(x - size, y + offset) && get_pixel(image, x - size, y + offset) || // Left column
            in_bounds(x + size, y + offset) && get_pixel(image, x + size, y + offset))   // Right column
        {
            white_in_frame += 1;
        }
    }
    return white_in_frame;
}