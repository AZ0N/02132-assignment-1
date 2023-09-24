#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cbmp.h"
#include "library.h"

// TODO Remove!

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

unsigned char KERNEL_5x5[5][5] = {
    {1, 4, 6, 4, 1},
    {4, 16, 24, 16, 4},
    {6, 24, 36, 24, 6},
    {4, 16, 24, 16, 4},
    {1, 4, 6, 4, 1}};

unsigned char KERNEL_TEST[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}};

void convolve(unsigned char image[BMP_WIDTH][BMP_HEIGTH], unsigned char output[BMP_WIDTH][BMP_HEIGTH], unsigned char kernel[5][5])
{
  int sum = 0;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      // if (!image[x][y])
      // {
      //   continue;
      // }
      // printf("Pixel: %d, %d\n", x, y);
      sum = 0;
      for (int dx = max(0, x - 2); dx < min(BMP_WIDTH, x + 3); dx++)
      {
        for (int dy = max(0, y - 2); dy < min(BMP_HEIGTH, y + 3); dy++)
        {
          sum += (image[dx][dy]) ? kernel[dx - x + 2][dy - y + 2] : 0;
        }
      }

      if (sum > 8) {
        output[x][y] = 255;
      }
      else
      {
        output[x][y] = 0;
      }
      // Use >> 8 instead of / 256
      // output[x][y] = (sum - 1) / 256;
      // output[x][y] = (output[x][y] > 190) ? 255 : 0;
    }
  }
}

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

  //   char filename[30];
  //   sprintf(filename, "./output/erode%d.bmp", erode_number);
  single_to_multi_channel(working_image[0], temp);
  write_bitmap(temp, "./output/test/grayscale.bmp");

  int from_index = 0;
  int to_index = 1;

  for (int i = 0; i < 10; i++)
  {
    convolve(working_image[from_index], working_image[to_index], KERNEL_TEST);
    single_to_multi_channel(working_image[to_index], temp);
    char filename[30];
    sprintf(filename, "./output/test/convolve_%d.bmp", i);
    write_bitmap(temp, filename);

    from_index = (from_index + 1) % 2;
    to_index = (to_index + 1) % 2;
  }

  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      working_image[from_index][x][y] = (working_image[from_index][x][y] > 220) ? 255 : 0;
    }
  }

  single_to_multi_channel(working_image[from_index], temp);
  write_bitmap(temp, "./output/test/threshold.bmp");
  // int erode_number = 0;
  // int from_index = 0;
  // int to_index = 1;

  // clock_t start, end;
  // double cpu_time;

  // start = clock();
  // // Erode while something was eroded in the last iteration
  // while (erode(working_image[from_index], working_image[to_index], erode_number))
  // {
  //   end = clock();

  //   cpu_time = end - start;
  //   printf("Erode %d: %f ms\n", erode_number, cpu_time * 1000 / CLOCKS_PER_SEC);
  //   // Save erode image
  //   char filename[30];
  //   sprintf(filename, "./output/erode%d.bmp", erode_number);
  //   single_to_multi_channel(working_image[to_index], temp);
  //   write_bitmap(temp, filename);

  //   // Detect cells in the eroded image
  //   start = clock();
  //   detect(working_image[to_index], input_image, &number_of_cells);
  //   end = clock();
  //   cpu_time = end - start;
  //   printf("Detect %d: %f ms\n", erode_number, cpu_time * 1000 / CLOCKS_PER_SEC);

  //   // Increment the erode_number
  //   erode_number++;

  //   // Increment and wrap from and to indexes (0 -> 1, 1 -> 0)
  //   from_index = (from_index + 1) % 2;
  //   to_index = (to_index + 1) % 2;

  //   // Save detect image
  //   // sprintf(filename, "./output/detect_%d.bmp", erode_number);
  //   // write_bitmap(input_image, filename);

  //   start = clock();
  // }
  // write_bitmap(input_image, argv[2]);
  // printf("%d cells found.\n", number_of_cells);
  return 0;
}
