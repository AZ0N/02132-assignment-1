#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

const int THRESHOLD = 100;

// Prototypes
void grayscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
void threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
void single_to_multi_channel(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
void erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

// TODO Use input_image & output_image channels instead of separate arrays
unsigned char working_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char working_image_2[BMP_WIDTH][BMP_HEIGTH];

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
    fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
    exit(1);
  }

  printf("Example program - 02132 - A1\n");

  // Load image from file
  read_bitmap(argv[1], input_image);

  // Convert to grayscale
  grayscale(input_image, working_image);

  // Threshold
  threshold(working_image);

  for (int i = 0; i < 12; i++)
  {
    // Erode
    erode(working_image, working_image_2);

    for (int x = 0; x < BMP_WIDTH; x++)
    {
      for (int y = 0; y < BMP_HEIGTH; y++)
      {
        working_image[x][y] = working_image_2[x][y];
      }
    }

    // Back to multi channel
    single_to_multi_channel(working_image, output_image);

    // Set filename
    char filename[30];
    sprintf(filename, "./erode-%d.bmp", i);

    // Save image to file
    write_bitmap(output_image, filename);
  }

  printf("Done!\n");
  return 0;
}

void grayscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH])
{
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      unsigned char average = (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) / 3;
      output_image[x][y] = average;
    }
  }
}

void threshold(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH])
{
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      input_image[x][y] = input_image[x][y] > THRESHOLD ? 255 : 0;
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

void erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH])
{
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
      }
      else
      {
        output_image[x][y] = 0;
      }
    }
  }
}