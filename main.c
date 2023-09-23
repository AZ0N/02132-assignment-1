#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cbmp.h"
#include "library.h"

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char working_image[3][BMP_WIDTH][BMP_HEIGTH];

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

  clock_t start, end;
  double cpu_separate, cpu_division, cpu_final;

  // Measure time usage of three methods
  start = clock();
  grayscale_and_threshold_separate(input_image, working_image[0]);
  end = clock();
  cpu_separate = end - start;

  start = clock();
  grayscale_and_threshold_division(input_image, working_image[1]);
  end = clock();
  cpu_division = end - start;

  start = clock();
  grayscale_and_threshold(input_image, working_image[2]);
  end = clock();
  cpu_final = end - start;

  // Check wether the resulting images are the same.
  int dif_sep_div = 0;
  int dif_div_fin = 0;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
      for (int y = 0; y < BMP_HEIGTH; y++)
      {
        dif_sep_div += (working_image[0][x][y] != working_image[1][x][y]) ? 1 : 0;
        dif_div_fin += (working_image[1][x][y] != working_image[2][x][y]) ? 1 : 0;
      }
  }
  printf("%s; %f; %f; %f; %d; %d\n", argv[1], cpu_separate * 1000.0 / CLOCKS_PER_SEC, cpu_division * 1000.0 / CLOCKS_PER_SEC, cpu_final * 1000.0 / CLOCKS_PER_SEC, dif_sep_div, dif_div_fin);

  return 0;
}
