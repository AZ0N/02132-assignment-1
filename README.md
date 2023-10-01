# 02132 Assignment 1 - Cell detection

## Instructions 
### Linux/Mac
```bash
gcc library.c erode.c cbmp.c main.c -o main.out -std=c99
./main.out input.bmp output.bmp
```
### Windows
```bash
gcc library.c erode.c cbmp.c main.c -o main.exe -std=c99
.\main.exe input.bmp output.bmp
```

## Configuration
### `main.c`
```c
// Controls whether erode and detect images are saved to ./output/
// #define SAVE_ERODE
// #define SAVE_DETECT
```
If `SAVE_ERODE` or `SAVE_DETECT` is defined, erode and detect images will be saved at `./output/`. Therefore the folder need to exist.

### `library.c`
```c
// Controls whether the centers of cells are printed to standard out
#define PRINT_CENTERS
```