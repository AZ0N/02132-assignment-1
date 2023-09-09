#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

#define THRESHOLD 90
#define CROSS_RADIUS 8

// Prototypes
void grayscale_and_threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
void single_to_multi_channel(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
void detect(unsigned char image[BMP_WIDTH][BMP_HEIGTH]);
void draw_cross(int x, int y, unsigned char image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char working_image[2][BMP_WIDTH][BMP_HEIGTH];

// The total number of detected cells
int number_cells = 0;

// Main function
int main(int argc, char **argv)
{
  // argc counts how may arguments are passed
  // argv[0] is a string with the name of the program
  // argv[1] is the first command line argument (input image)
  // argv[2] is the second command line argument (output image)

  // Checking that 2 arguments are passed
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
    exit(1);
  }

  // Load image from file
  read_bitmap(argv[1], input_image);

  // Convert to grayscale and apply threshold
  grayscale_and_threshold(input_image, working_image[0]);

  int erode_number = 0;
  int from_index = 0;
  int to_index = 1;

  while (1)
  {
    // Erode, and break if nothing was eroded (0! = true)
    if (!erode(working_image[from_index], working_image[to_index]))
    {
      break;
    }
    // Detect cells in the eroded image
    detect(working_image[to_index]);

    // Increment the erode_number
    erode_number++;

    // Increment and wrap from and to indexes (0 -> 1, 1 -> 0)
    from_index = (from_index + 1) % 2;
    to_index = (to_index + 1) % 2;

    // Save erode step image
    // char filename[30];
    // sprintf(filename, "./erode_and_detect_%d.bmp", erode_number);
    // write_bitmap(input_image, filename);
  }
  write_bitmap(input_image, argv[2]);
  printf("%d cells found.\n", number_cells);
  return 0;
}

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

int erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH])
{
  int did_erode = 0;

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (input_image[x][y] &&                             // Is this pixel white?
          (y > 0 && input_image[x][y - 1]) &&              // Is the above pixel white?
          (x > 0 && input_image[x - 1][y]) &&              // Is the left pixel white?
          (y < BMP_HEIGTH - 1 && input_image[x][y + 1]) && // Is the bottom pixel white?
          (x < BMP_WIDTH - 1 && input_image[x + 1][y]))    // Is the right pixel white?
      {
        output_image[x][y] = 255;
        did_erode = 1;
      }
      else
      {
        output_image[x][y] = 0;
      }
    }
  }
  return did_erode;
}

void detect(unsigned char image[BMP_WIDTH][BMP_HEIGTH])
{
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      int white_in_exclusion = 0;
      // First check exclusion frame
      // TODO Use constant for exlusion and detection frame size
      for (int d = -6; d <= 6; d++)
      {
        // Vertical lines
        if (0 <= x + d && x + d < BMP_WIDTH)
        {
          // Top row
          if (0 <= y - 6 && image[x + d][y - 6])
          {
            white_in_exclusion = 1;
            break;
          }
          // Bottom row
          if (y + 6 < BMP_HEIGTH && image[x + d][y + 6])
          {
            white_in_exclusion = 1;
            break;
          }
        }
        // Horizontal lines
        if (0 <= y + d && y + d < BMP_HEIGTH)
        {
          // Left column
          if (0 <= x - 6 && image[x - 6][y + d])
          {
            white_in_exclusion = 1;
            break;
          }
          // Right column
          if (x + 6 < BMP_WIDTH && image[x + 6][y + d])
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
      int found_cell = 0;

      for (int dx = -5; dx <= 5; dx++)
      {
        for (int dy = -5; dy <= 5; dy++)
        {
          if (0 <= x + dx && x + dx < BMP_WIDTH && 0 <= y + dy && y + dy < BMP_HEIGTH && image[x + dx][y + dy])
          {
            found_cell = 1;
            image[x + dx][y + dy] = 0;
          }
        }
      }
      // If we found a cell
      if (found_cell)
      {
        printf("Cell: (%d, %d)\n", x, y);
        number_cells++;
        draw_cross(x, y, input_image);
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