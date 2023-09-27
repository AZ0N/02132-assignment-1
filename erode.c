#include "erode.h"

unsigned char byte_above(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_below(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_above_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_above_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_below_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);
unsigned char byte_below_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y);

// Erodes input[][] into outout[][]. Erodes in straight cross when iteration is even, and diagonal cross when odd. Returns 1 if any pixels were eroded, else 0
int erode(unsigned char input[PACKED_WIDTH][BMP_HEIGTH], unsigned char output[PACKED_WIDTH][BMP_HEIGTH], int iteration)
{
    int did_erode = 0;
    // Loop through each byte in the packed array
    for (int x = 0; x < PACKED_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            // Save value before erode
            unsigned char initial = input[x][y];
            if (iteration % 2 == 0)
            {
                // Erode in straight cross
                output[x][y] = input[x][y] &
                               byte_above(input, x, y) &
                               byte_below(input, x, y) &
                               byte_left(input, x, y) &
                               byte_right(input, x, y);
            }
            else
            {
                // Erode in diagonal cross
                output[x][y] = input[x][y] &
                               byte_above_left(input, x, y) &
                               byte_above_right(input, x, y) &
                               byte_below_left(input, x, y) &
                               byte_below_right(input, x, y);
            }
            // If any bits changed, we have eroded
            if (output[x][y] != initial)
            {
                did_erode = 1;
            }
        }
    }
    return did_erode;
}

// Returns the byte above image[x][y]. If (x,y) is in the top row, it returns 0xFF
unsigned char byte_above(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (0 < y) ? image[x][y - 1] : 0xFF;
}

// Returns the byte below image[x][y]. If (x,y) is in the bottom row, it returns 0xFF
unsigned char byte_below(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (y < BMP_HEIGTH - 1) ? image[x][y + 1] : 0xFF;
}

// Returns image[x][y] bitshifted 1 to the right. The MSB of the return value is the LSB of the left byte (1 if (x,y) is in the first column)
unsigned char byte_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (image[x][y] >> 1) | ((x == 0) ? 0x80 : (image[x - 1][y] << 7));
}

// Returns image[x][y] bitshifted 1 to the left. The LSB of the return value is the MSB of the right byte (1 if (x,y) is in the last column)
unsigned char byte_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (image[x][y] << 1) | ((x == PACKED_WIDTH - 1) ? 0x01 : (image[x + 1][y] >> 7));
}

// Returns byte_left() of the byte above. Returns 0xFF if there is no byte above
unsigned char byte_above_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (0 < y) ? byte_left(image, x, y - 1) : 0xFF;
}

// Returns byte_right() of the byte above. Returns 0xFF if there is no byte above
unsigned char byte_above_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (0 < y) ? byte_right(image, x, y - 1) : 0xFF;
}

// Returns byte_left() of the byte below. Returns 0xFF if there is no byte below
unsigned char byte_below_left(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (y < BMP_HEIGTH - 1) ? byte_left(image, x, y + 1) : 0xFF;
}

// Returns byte_right() of the byte below. Returns 0xFF if there is no byte below
unsigned char byte_below_right(unsigned char image[PACKED_WIDTH][BMP_HEIGTH], int x, int y)
{
    return (y < BMP_HEIGTH - 1) ? byte_right(image, x, y + 1) : 0xFF;
}