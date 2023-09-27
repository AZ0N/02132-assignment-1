#include <stdio.h>
#include "library.h"
#include <stdbool.h>

#define THRESHOLD 90
#define CROSS_RADIUS 8
#define DETECTION_HALF_RADIUS 5
#define MIN_WHITE_PIXELS 3
#define MAX_WHITE_IN_EXCLUSION 2
#define LINE_LIMIT 1

// Computing THRESHOLD * 3/4 so the average in grayscale_and_threshold can be computed division by 4 (using bitshift, << 2).
static const unsigned char THRESHOLD_SHIFTED = THRESHOLD * 3 / 4;

// Prototypes
int match_straight(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
int match_diagonal(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
void draw_cross(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int in_bounds(int x, int y);
int check_frame(unsigned char image[BMP_WIDTH][BMP_HEIGTH], int x, int y, int size);
bool isLine(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH]);

void grayscale_and_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            // Calculate and check (r + g + b) / 4 > THRESHOLD * 3 / 4
            unsigned char average_shifted = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) >> 2;
            output_image[x][y] = (average_shifted > THRESHOLD_SHIFTED) ? 255 : 0;
        }
    }
}

void single_to_multi_channel(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            for (int c = 0; c < BMP_CHANNELS; c++)
            {
                output_image[x][y][c] = input_image[x][y];
            }
        }
    }
}

int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int iteration)
{
    int did_erode = 0;
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            if (input_image[x][y])
            {
                // If any of the directly surrounding pixels are black (edges are considered white) erode the pixel
                if ((iteration % 2 == 0 && match_straight(x, y, input_image)) || (iteration % 2 != 0 && match_diagonal(x, y, input_image)))
                {
                    output_image[x][y] = 0;
                    did_erode = 1;
                }
                else
                {
                    output_image[x][y] = 255;
                }
            }
            else
            {
                output_image[x][y] = 0;
            }
        }
    }
    return did_erode;
}

int match_straight(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH])
{
    return ((y > 0 && !input_image[x][y - 1]) ||
            (x > 0 && !input_image[x - 1][y]) ||
            (y < BMP_HEIGTH - 1 && !input_image[x][y + 1]) ||
            (x < BMP_WIDTH - 1 && !input_image[x + 1][y]))
               ? 1
               : 0;
}

int match_diagonal(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH])
{
    return ((0 < x && 0 < y && !input_image[x - 1][y - 1]) ||
            (0 < x && y < BMP_HEIGTH - 1 && !input_image[x - 1][y + 1]) ||
            (x < BMP_WIDTH - 1 && 0 < y && !input_image[x + 1][y - 1]) ||
            (x < BMP_WIDTH - 1 && y < BMP_HEIGTH - 1 && !input_image[x + 1][y + 1]))
               ? 1
               : 0;
}

void detect(unsigned char image[BMP_WIDTH][BMP_HEIGTH], unsigned char draw_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned short *number_of_cells)
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            int white_in_exclusion_inner = check_frame(image, x, y, DETECTION_HALF_RADIUS + 1);
            int white_in_exclusion_outer = check_frame(image, x, y, DETECTION_HALF_RADIUS + 2);

            // If we found a white pixel in the exclusion frame, move the detection windows 1 over
            if (white_in_exclusion_outer > MAX_WHITE_IN_EXCLUSION)
            {
                continue;
            }
            if (white_in_exclusion_inner > MAX_WHITE_IN_EXCLUSION)
            {
                continue;
            }
            // If we found 3 or less white pixels in excliosn frame, check if they are connected/lined up
            if (white_in_exclusion_inner <= MAX_WHITE_IN_EXCLUSION)
            {
                int white_in_frame = 0;
                int size = DETECTION_HALF_RADIUS + 1;
                // First check exclusion frame
                for (int offset = -size; offset <= size; offset++)
                {
                    if (in_bounds(x + offset, y - size) && image[x + offset][y - size] || // Top row
                        in_bounds(x + offset, y + size) && image[x + offset][y + size] || // Bottom row
                        in_bounds(x - size, y + offset) && image[x - size][y + offset] || // Left column
                        in_bounds(x + size, y + offset) && image[x + size][y + offset])   // Right column
                    {
                        white_in_frame += 1;
                        // If white pixel in exclusion frame is part of line, remove it
                        if (in_bounds(x + offset, y - size) && isLine(x + offset, y - size, image))
                        {
                            image[x + offset][y - size] = 0;
                        }
                    }
                    // When all the white pixels are checked, stop
                    if (white_in_frame == white_in_exclusion_inner)
                    {
                        continue; // TODO might not work like this, needs to go to loop for main frame
                    }
                }
            }
            // Exclusion frame is all black (or empty), check if any white pixels inside
            int white_pixels_found = 0;

            for (int dx = -DETECTION_HALF_RADIUS; dx <= DETECTION_HALF_RADIUS; dx++)
            {
                for (int dy = -DETECTION_HALF_RADIUS; dy <= DETECTION_HALF_RADIUS; dy++)
                {
                    if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH && image[x + dx][y + dy])
                    {
                        white_pixels_found += 1;
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
                        if (in_bounds(x + dx, y + dy) && image[x + dx][y + dy])
                        {
                            image[x + dx][y + dy] = 0;
                        }
                    }
                }
#ifdef PRINT_CELLS
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
    // Horizontal line
    for (int dx = -CROSS_RADIUS; dx <= CROSS_RADIUS; dx++)
    {
        for (int dy = -2; dy <= 2; dy++)
        {
            if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH)
            {
                image[x + dx][y + dy][0] = 255;
                image[x + dx][y + dy][1] = image[x + dx][y + dy][2] = 0;
            }
        }
    }
    // Vertical line
    for (int dy = -CROSS_RADIUS; dy <= CROSS_RADIUS; dy++)
    {
        for (int dx = -2; dx <= 2; dx++)
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

int check_frame(unsigned char image[BMP_WIDTH][BMP_HEIGTH], int x, int y, int size)
{
    int white_in_frame = 0;
    // First check exclusion frame
    for (int offset = -size; offset <= size; offset++)
    {
        if (in_bounds(x + offset, y - size) && image[x + offset][y - size] || // Top row
            in_bounds(x + offset, y + size) && image[x + offset][y + size] || // Bottom row
            in_bounds(x - size, y + offset) && image[x - size][y + offset] || // Left column
            in_bounds(x + size, y + offset) && image[x + size][y + offset])   // Right column
        {
            white_in_frame += 1;
        }
    }
    return white_in_frame;
}

// Function to check if a group of white pixels form a straight or diagonal line
bool isLine(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH])
{
    int LINE_LENGHT_STRAIGHT = 0;
    int LINE_LENGHT_DIAGONAL = 0;

    // Check for a straight line pattern
    if ((y > 0 && input_image[x][y - 1] == 0) && (y < BMP_HEIGTH - 1 && input_image[x][y + 1] == 0))
    {
        LINE_LENGHT_STRAIGHT++;
        if (LINE_LENGHT_STRAIGHT >= LINE_LIMIT)
        {
            return true; // Horizontal line
        }
    }
    if ((x > 0 && input_image[x - 1][y] == 0) && (x < BMP_WIDTH - 1 && input_image[x + 1][y] == 0))
    {
        LINE_LENGHT_STRAIGHT++;
        if (LINE_LENGHT_STRAIGHT >= LINE_LIMIT)
        {
            return true; // Vertical line
        }
    }

    // Check for a diagonal line pattern
    if ((x > 0 && y > 0 && input_image[x - 1][y - 1] == 0) && (x < BMP_WIDTH - 1 && y < BMP_HEIGTH - 1 && input_image[x + 1][y + 1] == 0))
    {
        LINE_LENGHT_DIAGONAL++;
        if (LINE_LENGHT_DIAGONAL >= LINE_LIMIT)
        {
            return true; // Diagonal (\) line
        }
    }
    if ((x > 0 && y < BMP_HEIGTH - 1 && input_image[x - 1][y + 1] == 0) && (x < BMP_WIDTH - 1 && y > 0 && input_image[x + 1][y - 1] == 0))
    {
        LINE_LENGHT_DIAGONAL++;
        if (LINE_LENGHT_DIAGONAL >= LINE_LIMIT)
        {
            return true; // Diagonal (/) line
        }
    }

    return false; // No line pattern detected
}
