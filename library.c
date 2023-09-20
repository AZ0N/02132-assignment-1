#include "library.h"

void grayscale_and_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH])
{
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            unsigned char average = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
            output_image[x][y] = (average > THRESHOLD) ? 255 : 0;
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
            int white_in_exclusion = 0;
            // First check exclusion frame
            for (int d = -(DETECTION_HALF_RADIUS + 1); d <= (DETECTION_HALF_RADIUS + 1); d++)
            {
                // Vertical lines
                if (0 <= x + d && x + d < BMP_WIDTH)
                {
                    // Top row
                    if (0 <= y - (DETECTION_HALF_RADIUS + 1) && image[x + d][y - (DETECTION_HALF_RADIUS + 1)])
                    {
                        white_in_exclusion = 1;
                        break;
                    }
                    // Bottom row
                    if (y + (DETECTION_HALF_RADIUS + 1) < BMP_HEIGTH && image[x + d][y + (DETECTION_HALF_RADIUS + 1)])
                    {
                        white_in_exclusion = 1;
                        break;
                    }
                }
                // Horizontal lines
                if (0 <= y + d && y + d < BMP_HEIGTH)
                {
                    // Left column
                    if (0 <= x - (DETECTION_HALF_RADIUS + 1) && image[x - (DETECTION_HALF_RADIUS + 1)][y + d])
                    {
                        white_in_exclusion = 1;
                        break;
                    }
                    // Right column
                    if (x + (DETECTION_HALF_RADIUS + 1) < BMP_WIDTH && image[x + (DETECTION_HALF_RADIUS + 1)][y + d])
                    {
                        white_in_exclusion = 1;
                        break;
                    }
                }
            }
            // If we found a white pixel in the exclusion frame, move the detection windows 1 over
            if (white_in_exclusion)
            {
                continue;
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
                        image[x + dx][y + dy] = 0;
                    }
                }
            }
            // If we found enough white pixels, we detected a cell
            if (white_pixels_found > MIN_WHITE_PIXELS)
            {
                // printf("Cell: (%d, %d)\n", x, y);
                *number_of_cells += 1;
                draw_cross(x, y, draw_image);
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