#include <stdlib.h>
#include <stdio.h>

#include "cbmp.h"
#include "library.h"

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char working_image[2][BMP_WIDTH][BMP_HEIGTH];
// TODO Remove. Used for saving erode steps
unsigned char temp[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

// The total number of detected cells
unsigned short number_of_cells = 0;

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

  int iteration = 0;
  int from_index = 0;
  int to_index = 1;

  // Erode while something was eroded in the last iteration
  while (erode(working_image[from_index], working_image[to_index], iteration))
  {
    // Save erode image
    // char filename[30];
    // sprintf(filename, "./output/erode%d.bmp", erode_number);
    // single_to_multi_channel(working_image[to_index], temp);
    // write_bitmap(temp, filename);

    if (iteration > 2)
    {
      // Detect cells in the eroded image
      detect(working_image[to_index], input_image, &number_of_cells);
    }

    // Increment the erode_number
    iteration++;

    // Increment and wrap from and to indexes (0 -> 1, 1 -> 0)
    from_index = (from_index + 1) % 2;
    to_index = (to_index + 1) % 2;

    // Save detect image
    // sprintf(filename, "./output/detect_%d.bmp", erode_number);
    // write_bitmap(input_image, filename);
  }
  write_bitmap(input_image, argv[2]);
  printf("%s; %d\n", argv[1], number_of_cells);
  return 0;
}
