#include <stdlib.h>
#include <stdio.h>

#include "cbmp.h"
#include "library.h"

// Use to control whether erode and detects images are saved to ./output/
// #define SAVE_ERODE
// #define SAVE_DETECT

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char working_image[2][BMP_WIDTH][BMP_HEIGTH];

#ifdef SAVE_ERODE
// Declare temp buffer to save erode image in, if it should be outputted
unsigned char temp[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
#endif

#if defined(SAVE_ERODE) || defined(SAVE_DETECT)
// Declare buffer to save file path, if erode and/or detect steps should be saved
char filename[30];
#endif

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
#ifdef SAVE_ERODE
    // Save erode image
    sprintf(filename, "./output/erode%d.bmp", iteration);
    single_to_multi_channel(working_image[to_index], temp);
    write_bitmap(temp, filename);
#endif

    if (iteration > 2)
    {
      // Detect cells in the eroded image
      detect(working_image[to_index], input_image, &number_of_cells);
#ifdef SAVE_DETECT
      // Save detect image
      sprintf(filename, "./output/detect_%d.bmp", iteration);
      write_bitmap(input_image, filename);
#endif
    }

    // Increment the iteration number
    iteration++;

    // Increment and wrap from and to indexes (0 -> 1, 1 -> 0)
    from_index = (from_index + 1) % 2;
    to_index = (to_index + 1) % 2;
  }
  write_bitmap(input_image, argv[2]);
  printf("%d cells found.\n", number_of_cells);
  return 0;
}
