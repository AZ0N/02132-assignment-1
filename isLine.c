#include <stdbool.h>
#include "library.h"

int LINE_LIMIT = 3;

// Function to check if a group of white pixels form a straight or diagonal line
bool isLine(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH])
{
    int LINE_LENGHT = 0;

    // Check if the central pixel is white
    if (input_image[x][y] == 0)
    {
        return false; // It's not part of a line
    }

    // Check for a straight line pattern
    if ((y > 0 && input_image[x][y - 1] == 0) && (y < BMP_HEIGTH - 1 && input_image[x][y + 1] == 0))
    {
        LINE_LENGHT++;
        if (LINE_LENGHT >= LINE_LIMIT)
        {
            return true; // Horizontal line 
        }
    }
    if ((x > 0 && input_image[x - 1][y] == 0) && (x < BMP_WIDTH - 1 && input_image[x + 1][y] == 0))
    {
        LINE_LENGHT++;
        if (LINE_LENGHT >= LINE_LIMIT)
        {
            return true; // Vertical line 
        }
    }

    // Check for a diagonal line pattern
    if ((x > 0 && y > 0 && input_image[x - 1][y - 1] == 0) && (x < BMP_WIDTH - 1 && y < BMP_HEIGTH - 1 && input_image[x + 1][y + 1] == 0))
    {
        LINE_LENGHT++;
        if (LINE_LENGHT >= LINE_LIMIT)
        {
            return true; // Diagonal (\) line 
        }
    }
    if ((x > 0 && y < BMP_HEIGTH - 1 && input_image[x - 1][y + 1] == 0) && (x < BMP_WIDTH - 1 && y > 0 && input_image[x + 1][y - 1] == 0))
    {
        LINE_LENGHT++;
        if (LINE_LENGHT >= LINE_LIMIT)
        {
            return true; // Diagonal (/) line 
        }
    }

    return false; // No line pattern detected
}
