#ifndef ERODE_H
#define ERODE_H

#include "cbmp.h"
#include "library.h"

int erode(unsigned char input[PACKED_WIDTH][BMP_HEIGTH], unsigned char output[PACKED_WIDTH][BMP_HEIGTH], int iteration);

#endif // ERODE_H