/*
  process_pgm.c - Loads, modifies and saves a PGM image file

  Written by Ted Burke - last modified 18-11-2011

  Included image processing operations:

  1. invert colours
  2. threshold
  3. Count white pixels
  4. horizontal edge detect
  5. vertical edge detect

  To compile:

     gcc process_pgm.c -o process_pgm.exe
*/
 
#include <stdio.h>
#include <assert.h>
 
// 2-D pixel array, like a spreadsheet with 1000 rows & 1000 columns
int p[1000][1000];
int w, h;
 
// Image file functions
void load_image_from_file(char[]);
void save_image_to_file(char[]);
 
// Image processing functions
void invert_colours();
void apply_threshold(int);
int count_white_pixels();
void horizontal_edge_detect(int);
void vertical_edge_detect(int);

int main()
{
  // Try out the invert colours function
  load_image_from_file("input.pgm");
  invert_colours();
  save_image_to_file("inverted_image.pgm");
 
  // Try out the image threshold function
  load_image_from_file("input.pgm");
  apply_threshold(127);
  save_image_to_file("thresholded_image.pgm");
 
  int edge_threshold = 30;
 
  // Horizontal edge detect
  load_image_from_file("input.pgm");
  horizontal_edge_detect(edge_threshold);
  save_image_to_file("horizontal_edges.pgm");
 
  // Vertical edge detect
  load_image_from_file("input.pgm");
  vertical_edge_detect(edge_threshold);
  save_image_to_file("vertical_edges.pgm");

  // exit normally
  return 0;
}
 
void load_image_from_file(char filename[])
{
  // Variables
  FILE *f;
  char line[200];
  int x, y;
 
  // Open input image file
  printf("Opening input file %s\n", filename);
  f = fopen(filename, "r");
 
  // Read 4 header lines.
  // These will just be ignored, but we need to read
  // them to get down to the table of numbers further
  // down in the file.
  printf("Reading file header info\n");
  assert(1 == fscanf(f, "%[^\n]\n", line));
  assert(1 == fscanf(f, "%[^\n]\n", line));
  assert(2 == fscanf(f, "%d %d\n", &w, &h));
  assert(1 == fscanf(f, "%[^\n]\n", line));
 
  // Read pixel data into 2-D array
  printf("Reading pixel data from file %s\n", filename);
  y = 0;
  while (y < h) {
    x = 0;
    while (x < w) {
      assert (1 == fscanf(f, "%d", &p[y][x]));
      x = x + 1;
    }
    y = y + 1;
  }
 
  // Close input file
  printf("Closing input file %s\n", filename);
  fclose(f);
}
 
void save_image_to_file(char filename[])
{
  // variables
  FILE *f;
  int x, y;
 
  // open output file
  printf("Opening output file %s\n", filename);
  f = fopen(filename, "w");
 
  // Print header info into file
  printf("Printing file header to file %s\n", filename);
  fprintf(f, "P2\n");
  fprintf(f, "# My PGM file\n");
  fprintf(f, "%d %d\n", w, h);
  fprintf(f, "255\n");
 
  // Print pixel data into file
  printf("Printing pixel data to file %s\n", filename);
  y = 0;
  while (y < h) {
    x = 0;
    while (x < w) {
      fprintf(f, "%d ", p[y][x]);
      x = x + 1;
    }
    fprintf(f, "\n");
    y = y + 1;
  }
 
  // Close file
  printf("Closing output file %s\n", filename);
  fclose(f);
}
 
void invert_colours(int threshold)
{
  // variables
  int x, y;
 
  // Modify all pixel values one at a time
  printf("Inverting colours...");
  y = 0;
  while (y < h) {
    x = 1;
    while (x < w) {
      p[y][x] = 255 - p[y][x];
      x = x + 1;
    }
    y = y + 1;
  }
  printf("OK\n");
}
 
void apply_threshold(int threshold)
{
  // variables
  int x, y;
 
  // Modify all pixel values one at a time
  printf("Applying threshold...");
  y = 0;
  while (y < h) {
    x = 0;
    while (x < w) {
      if (p[y][x] > threshold) {
	p[y][x] = 255;
      }
      else {
	p[y][x] = 0;
      }
      x = x + 1;
    }
    y = y + 1;
  }
  printf("OK\n");
}
 
int count_white_pixels()
{
  // variables
  int x, y;
  int counter = 0;
 
  printf("Counting white pixels...");
  y = 0;
  while (y < h) {
    x = 1;
    while (x < w) {
      if (p[y][x] == 255) {
	counter = counter + 1;
      }
      x = x + 1;
    }
    y = y + 1;
  }
  printf("OK\n");
 
  return counter;
}
 
void horizontal_edge_detect(int min_diff)
{
  // variables
  int x, y;
  int diff;
 
  // Any pixel that differs from its right-hand neighbour
  // by more than min_diff will become white. Other pixels
  // become black.
  printf("Detecting horizontal edges...");
 
  y = 0;
  while (y < h) {
    x = 0;
    while (x < w-1) {
      // Find the difference in colour between
      // this pixel and the one to the right of it
      diff = p[y][x+1] - p[y][x];
 
      // Make sure difference is positive
      if (diff < 0) diff = -diff;
 
      // If the difference exceeds the threshold
      // make the pixel white
      if (diff > min_diff) p[y][x] = 255;
      else p[y][x] = 0;
 
      x = x + 1;
    }
    y = y + 1;
  }
  printf("OK\n");
}
 
void vertical_edge_detect(int min_diff)
{
  // variables
  int x, y;
  int diff;
 
  // Any pixel that differs from the one immediately below
  // it by more than min_diff will become white. All other
  // pixels become black.
  printf("Detecting vertical edges...");
 
  y = 0;
  while (y < h-1) {
    x = 0;
    while (x < w) {
      // Find the difference in colour between
      // this pixel and the one below it
      diff = p[y+1][x] - p[y][x];
 
      // Make sure difference is positive
      if (diff < 0) diff = -diff;
 
      // If the difference exceeds the threshold
      // make the pixel white
      if (diff > min_diff) p[y][x] = 255;
      else p[y][x] = 0;
 
      x = x + 1;
    }
    y = y + 1;
  }
  printf("OK\n");
}
