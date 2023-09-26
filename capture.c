#include "library.h"
#include <stdbool.h>

#define DETECTION_HALF_RADIUS 5
#define MIN_WHITE_PIXELS 3
#define MAX_WHITE_IN_EXCLUSION 3
#define LINE_LIMIT 1

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
                        if (in_bounds(x + offset, y - size) && isLine(x + offset, y - size, image[BMP_WIDTH][BMP_HEIGTH]))
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
                // printf("Cell: (%d, %d)\n", x, y);
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
