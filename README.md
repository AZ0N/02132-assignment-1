# 02132 Assignment 1 - Cell detection

## Instructions 
### Linux/Mac
```bash
gcc library.y erode.c cbmp.c main.c -o main.out -std=c99
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
// Controls whether erode and detects images are saved to ./output/
// #define SAVE_ERODE
// #define SAVE_DETECT
```

### `library.c`
```c
// Controls whether the centers of cells are printed to standard out
#define PRINT_CENTERS
```