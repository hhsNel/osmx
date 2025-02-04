#ifndef CARD_RENDERER_H
#define CARD_RENDERER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 375
#define HEIGHT 523
#define HEADER_SIZE 16  // Farbfeld header size

// Simple structure for an image buffer
typedef struct {
    uint8_t pixels[HEIGHT][WIDTH][3];  // RGB only
} Image;

// Initialize the image with a background color
void init_image(Image *img, uint8_t r, uint8_t g, uint8_t b) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			img->pixels[y][x][0] = r;
			img->pixels[y][x][1] = g;
			img->pixels[y][x][2] = b;
		}
	}
}

// Draw a simple rectangle (border, text box, etc.)
void draw_rect(Image *img, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
	for (int y = y1; y <= y2; y++) {
		for (int x = x1; x <= x2; x++) {
			if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
				img->pixels[y][x][0] = r;
				img->pixels[y][x][1] = g;
				img->pixels[y][x][2] = b;
			}
		}
	}
}

// Write the image in Farbfeld format
void save_farbfeld(const char *filename, Image *img) {
	FILE *f = fopen(filename, "wb");
	if (!f) return;
	
	// Write header
	fputs("farbfeld", f);
	fputc(WIDTH / 16777216, f); fputc((WIDTH / 65536) % 256, f);
	fputc((WIDTH / 256) % 256, f); fputc(WIDTH % 256, f);

	fputc(HEIGHT / 16777216, f); fputc((HEIGHT / 65536) % 256, f);
	fputc((HEIGHT / 256) % 256, f); fputc(HEIGHT % 256, f);
	
	// Write pixel data (Farbfeld requires 16-bit per channel, so we duplicate bytes)
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			uint8_t r = img->pixels[y][x][0];
			uint8_t g = img->pixels[y][x][1];
			uint8_t b = img->pixels[y][x][2];
			
			fputc(r, f); fputc(r, f);
			fputc(g, f); fputc(g, f);
			fputc(b, f); fputc(b, f);
			fputc(255, f); fputc(255, f);  // Full alpha
		}
	}
	fclose(f);
}

#define MAX_STROKES 5
typedef struct {
	float startPos[2*MAX_STROKES];
	float endPos[2*MAX_STROKES];
} CharRender;

CharRender font[95] = {
	/* each character is 2 lines. The first line contains the starting positions,
	   the second contains ending positions. Each position contains 2 floating point
	   numbers, the X and Y coords. There are 5 positions in each line */
	/* 1st start/end	2nd start/end	3rd start/end	4th start/end	5th start/end */
	{ {0, 0,			0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0, 0,			0, 0,			0, 0,			0, 0,			0, 0} },	// space
	{ {0.5, 0.9,		0.5, 0.2,		0, 0,			0, 0,			0, 0}, 
	  {0.5, 0.3,		0.5, 0.1,		0, 0,			0, 0,			0, 0} },	// !
	{ {0.4, 0.6,		0.6, 0.6,		0, 0,			0, 0,			0, 0}, 
	  {0.4, 0.4,		0.6, 0.4,		0, 0,			0, 0,			0, 0} },	// "
	{ {0.3, 0.1,		0.65, 0.1,		0.1, 0.3,		0.1, 0.7,		0, 0}, 
	  {0.35, 0.9,		0.7, 0.9,		0.9, 0.3,		0.9, 0.7,		0, 0} },	// #
	{ {0.9, 0.8,		0.1, 0.8,		0.9, 0.2,		0.4, 0.05,		0.6, 0.95},
	  {0.1, 0.8,		0.9, 0.2,		0.1, 0.2,		0.4, 0.95,		0.6, 0.05} },// $
	{ {0.1, 0.8,		0.8, 0.1,		0.1, 0.1,		0, 0,			0, 0}, 
	  {0.2, 0.7,		0.7, 0.2,		0.9, 0.9,		0, 0,			0, 0} },	// %
	{ {0.9, 0.1,		0.1, 0.9,		0.9, 0.9,		0.1, 0.3,		0.3, 0.1}, 
	  {0.1, 0.9,		0.9, 0.9,		0.1, 0.3,		0.3, 0.1,		0.9, 0.3} },// &
	{ {0.5, 0.6,		0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0.5, 0.4,		0, 0,			0, 0,			0, 0,			0, 0} },	// '
	{ {0.2, 0.9,		0.1, 0.6,		0.1, 0.4,		0, 0,			0, 0}, 
	  {0.1, 0.6,		0.1, 0.4,		0.2, 0.1,		0, 0,			0, 0} },	// (
	{ {0.8, 0.9,		0.9, 0.6,		0.9, 0.4,		0, 0,			0, 0}, 
	  {0.9, 0.6,		0.9, 0.4,		0.8, 0.1,		0, 0,			0, 0} },	// )
	{ {0.5, 0.9,		0.4, 0.85,		0.4, 0.75,		0, 0,			0, 0}, 
	  {0.5, 0.7,		0.6, 0.75,		0.6, 0.85,		0, 0,			0, 0} },	// *
	{ {0.5, 0.3,		0.3, 0.5,		0, 0,			0, 0,			0, 0}, 
	  {0.5, 0.7,		0.7, 0.5,		0, 0,			0, 0,			0, 0} },	// +
	{ {0.1, 0.1,		0, 0,			0, 0,			0, 0,			0, 0},
	  {0.2, 0.3,		0, 0,			0, 0,			0, 0,			0, 0} },	// ,
	{ {0.3, 0.5,		0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0.7, 0.5,		0, 0,			0, 0,			0, 0,			0, 0} },	// -
	{ {0.5, 0.2,		0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0.5, 0.1,		0, 0,			0, 0,			0, 0,			0, 0} },	// .
	{ {0.4, 0.1,		0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0.6, 0.9,		0, 0,			0, 0,			0, 0,			0, 0} },	// /
	{ {0.2, 0.1,		0.8, 0.1,		0.8, 0.9,		0.2, 0.9,		0.2, 0.2},
	  {0.8, 0.1,		0.8, 0.9,		0.2, 0.9,		0.2, 0.1,		0.8, 0.8} },// 0
	{ {0.5, 0.9,		0.4, 0.1,		0.5, 0.9,		0, 0,			0, 0},
	  {0.5, 0.1,		0.6, 0.1,		0.3, 0.7,		0, 0,			0, 0} },	// 1
	{ {0.1, 0.1,		0.1, 0.1,		0.9, 0.8,		0.8, 0.9,		0.2, 0.9},
	  {0.9, 0.1,		0.9, 0.8,		0.8, 0.9,		0.2, 0.9,		0.1, 0.8} },// 2
	{ {0.1, 0.1,		0.9, 0.1,		0.9, 0.9,		0.9, 0.5,		0, 0},
	  {0.9, 0.1,		0.9, 0.9,		0.1, 0.9,		0.2, 0.5,		0, 0} },	// 3
	{ {0.7, 0.1,		0.7, 0.9,		0.1, 0.4,		0, 0,			0, 0},
	  {0.7, 0.9,		0.1, 0.4,		0.8, 0.4,		0, 0,			0, 0} },	// 4
	{ {0.9, 0.9,		0.1, 0.9,		0.1, 0.5,		0.8, 0.5,		0.9, 0.1},
	  {0.1, 0.9,		0.1, 0.5,		0.8, 0.5,		0.9, 0.1,		0.1, 0.1} },// 5
	{ {0.1, 0.1,		0.1, 0.1,		0.9, 0.1,		0.9, 0.5,		0.2, 0.9},
	  {0.2, 0.9,		0.9, 0.1,		0.9, 0.5,		0.15, 0.5,		0.9, 0.9} },// 6
	{ {0.1, 0.9,		0.9, 0.9,		0.7, 0.5,		0, 0,			0, 0},
	  {0.9, 0.9,		0.5, 0.1,		0.5, 0.5,		0, 0,			0, 0} },	// 7
	{ {0.1, 0.9,		0.9, 0.9,		0.9, 0.1,		0.1, 0.1,		0.1, 0.5},
	  {0.9, 0.9,		0.9, 0.1,		0.1, 0.1,		0.1, 0.9,		0.9, 0.5} },// 8
	{ {0.1, 0.9,		0.9, 0.9,		0.1, 0.9,		0.1, 0.5,		0.8, 0.1},
	  {0.9, 0.9,		0.8, 0.1,		0.1, 0.5,		0.85, 0.5,		0.1, 0.1} },// 9
	{ {0.5, 0.55,		0.5, 0.45,		0, 0,			0, 0,			0, 0},
	  {0.5, 0.65,		0.5, 0.35,		0, 0,			0, 0,			0, 0} },	// :
	{ {0.5, 0.55,		0.5, 0.45,		0, 0,			0, 0,			0, 0},
	  {0.5, 0.65,		0.4, 0.25,		0, 0,			0, 0,			0, 0} },	// ;
	{ {0.1, 0.5,		0.1, 0.5,		0, 0,			0, 0,			0, 0},
	  {0.9, 0.8,		0.9, 0.2,		0, 0,			0, 0,			0, 0} },	// <
	{ {0.1, 0.6,		0.9, 0.4,		0, 0,			0, 0,			0, 0},
	  {0.9, 0.6,		0.1, 0.4,		0, 0,			0, 0,			0, 0} },	// =
	{ {0.9, 0.5,		0.9, 0.5,		0, 0,			0, 0,			0, 0},
	  {0.1, 0.8,		0.1, 0.2,		0, 0,			0, 0,			0, 0} },	// >
	{ {0.5, 0.1,		0.5, 0.3,		0.5, 0.4,		0.9, 0.9,		0.2, 0.9},
	  {0.5, 0.2,		0.5, 0.4,		0.9, 0.9,		0.2, 0.9,		0.1, 0.8} },// ?
	{ {0.5, 0.1,		0.1, 0.5,		0.5, 0.9,		0.7, 0.3,		0.55, 0.35},
	  {0.1, 0.5,		0.5, 0.9,		0.7, 0.3,		0.4, 0.4,		0.45, 0.6} },// @
	{ {0.1, 0.1,		0.5, 0.9,		0.3, 0.5,		0, 0,			0, 0},
	  {0.5, 0.9,		0.9, 0.1,		0.7, 0.5,		0, 0,			0, 0} },	// A
	{ {0.1, 0.1,		0.1, 0.1,		0.9, 0.3,		0.9, 0.7,		0.9, 0.5},
	  {0.1, 0.9,		0.9, 0.3,		0.9, 0.7,		0.1, 0.9,		0.1, 0.5} },// B
	{ {0.9, 0.1,		0.2, 0.1,		0.1, 0.2,		0.1, 0.8,		0.2, 0.9},
	  {0.2, 0.1,		0.1, 0.2,		0.1, 0.8,		0.2, 0.9,		0.9, 0.9} },// C
	{ {0.1, 0.9,		0.1, 0.1,		0.7, 0.1,		0.9, 0.3,		0.9, 0.7},
	  {0.1, 0.1,		0.7, 0.1,		0.9, 0.3,		0.9, 0.7,		0.1, 0.9} },// D
	{ {0.9, 0.9,		0.1, 0.9,		0.1, 0.1,		0.1, 0.5,		0, 0},
	  {0.1, 0.9,		0.1, 0.1,		0.9, 0.1,		0.8, 0.5,		0, 0} },	// E
	{ {0.1, 0.1,		0.1, 0.9,		0.1, 0.5,		0, 0,			0, 0},
	  {0.1, 0.9,		0.9, 0.9,		0.5, 0.5,		0, 0,			0, 0} },	// F
	{ {0.9, 0.9,		0.1, 0.9,		0.1, 0.1,		0.9, 0.1,		0.9, 0.5},
	  {0.1, 0.9,		0.1, 0.1,		0.9, 0.1,		0.9, 0.5,		0.5, 0.5} },// G
	{ {0.1, 0.9,		0.9, 0.9,		0.1, 0.5,		0, 0,			0, 0},
	  {0.1, 0.1,		0.9, 0.1,		0.9, 0.5,		0, 0,			0, 0} },	// H
	{ {0.5, 0.9,		0.4, 0.1,		0.4, 0.9,		0, 0,			0, 0},
	  {0.5, 0.1,		0.6, 0.1,		0.6, 0.9,		0, 0,			0, 0} },	// I
	{ {0.1, 0.9,		0.9, 0.9,		0.9, 0.45,		0.7, 0.1,		0.3, 0.1},
	  {0.9, 0.9,		0.9, 0.45,		0.7, 0.1,		0.3, 0.1,		0.1, 0.3} },// J
	{ {0.1, 0.9,		0.1, 0.5,		0.1, 0.5,		0, 0,			0, 0},
	  {0.1, 0.1,		0.9, 0.9,		0.9, 0.1,		0, 0,			0, 0} },	// K
	{ {0.1, 0.9,		0.1, 0.1,		0, 0,			0, 0,			0, 0},
	  {0.1, 0.1,		0.9, 0.1,		0, 0,			0, 0,			0, 0} },	// L
	{ {0.1, 0.1,		0.1, 0.9,		0.5, 0.5,		0.9, 0.9,		0, 0},
	  {0.1, 0.9,		0.5, 0.5,		0.9, 0.9,		0.9, 0.1,		0, 0} },	// M
	{ {0.1, 0.1,		0.1, 0.9,		0.9, 0.1,		0, 0,			0, 0},
	  {0.1, 0.9,		0.9, 0.1,		0.9, 0.9,		0, 0,			0, 0} },	// N
	{ {0.1, 0.1,		0.9, 0.1,		0.9, 0.9,		0.1, 0.9,		0, 0},
	  {0.9, 0.1,		0.9, 0.9,		0.1, 0.9,		0.1, 0.1,		0, 0} },	// O
	{ {0.1, 0.1,		0.1, 0.9,		0.9, 0.9,		0.9, 0.5,		0, 0},
	  {0.1, 0.9,		0.9, 0.9,		0.9, 0.5,		0.1, 0.5,		0, 0} },	// P
	{ {0.1, 0.9,		0.1, 0.1,		0.9, 0.2,		0.9, 0.9,		0.9, 0.1},
	  {0.1, 0.1,		0.9, 0.2,		0.9, 0.9,		0.1, 0.9,		0.7, 0.3} },// Q
	{ {0.1, 0.1,		0.1, 0.9,		0.9, 0.9,		0.9, 0.5,		0.1, 0.5},
	  {0.1, 0.9,		0.9, 0.9,		0.9, 0.5,		0.1, 0.5,		0.9, 0.1} },// R
	{ {0.9, 0.9,		0.1, 0.8,		0.2, 0.5,		0.8, 0.5,		0.9, 0.2},
	  {0.1, 0.8,		0.2, 0.5,		0.8, 0.5,		0.9, 0.2,		0.1, 0.1} },// S
	{ {0.1, 0.9,		0.5, 0.9,		0, 0,			0, 0,			0, 0},
	  {0.9, 0.9,		0.5, 0.1,		0, 0,			0, 0,			0, 0} },	// T
	{ {0.1, 0.9,		0.1, 0.1,		0.9, 0.1,		0, 0,			0, 0},
	  {0.1, 0.1,		0.9, 0.1,		0.9, 0.9,		0, 0,			0, 0} },	// U
	{ {0.1, 0.9,		0.5, 0.1,		0, 0,			0, 0,			0, 0},
	  {0.5, 0.1,		0.9, 0.9,		0, 0,			0, 0,			0, 0} },	// V
	{ {0.1, 0.9,		0.2, 0.1,		0.5, 0.5,		0.8, 0.1,		0, 0},
	  {0.2, 0.1,		0.5, 0.5,		0.8, 0.1,		0.9, 0.9,		0, 0} },	// W
	{ {0.1, 0.9,		0.1, 0.1,		0, 0,			0, 0,			0, 0},
	  {0.9, 0.1,		0.9, 0.9,		0, 0,			0, 0,			0, 0} },	// X
	{ {0.1, 0.9,		0.5, 0.5,		0.5, 0.5,		0, 0,			0, 0},
	  {0.5, 0.5,		0.9, 0.9,		0.5, 0.1,		0, 0,			0, 0} },	// Y
	{ {0.1, 0.9,		0.9, 0.9,		0.1, 0.1,		0, 0,			0, 0},
	  {0.9, 0.9,		0.1, 0.1,		0.9, 0.1,		0, 0,			0, 0} },	// Z
	{ {0.3, 0.9,		0.1, 0.9,		0.1, 0.1,		0, 0,			0, 0},
	  {0.1, 0.9,		0.1, 0.1,		0.3, 0.1,		0, 0,			0, 0} },	// [
	{ {0.1, 0.9,		0, 0,			0, 0,			0, 0,			0, 0},
	  {0.9, 0.1,		0, 0,			0, 0,			0, 0,			0, 0} },	// backslash
	{ {0.7, 0.9,		0.9, 0.9,		0.9, 0.1,		0, 0,			0, 0},
	  {0.9, 0.9,		0.9, 0.1,		0.7, 0.1,		0, 0,			0, 0} },	// ]
	{ {0.5, 0.9,		0.5, 0.9,		0, 0,			0, 0,			0, 0},
	  {0.3, 0.7,		0.7, 0.7,		0, 0,			0, 0,			0, 0} },	// ^
	{ {0.1, 0.1,		0, 0,			0, 0,			0, 0,			0, 0},
	  {0.9, 0.1,		0, 0,			0, 0,			0, 0,			0, 0} },	// _
	{ {0.5, 0.9,		0, 0,			0, 0,			0, 0,			0, 0},
	  {0.6, 0.8,		0, 0,			0, 0,			0, 0,			0, 0} },	// `
	{ {0.9, 0.1,		0.9, 0.5,		0.9, 0.2,		0.2, 0.1,		0.1, 0.25},
	  {0.9, 0.5,		0.1, 0.4,		0.2, 0.1,		0.1, 0.25,		0.9, 0.35} },// a
	{ {0.1, 0.9,		0.1, 0.2,		0.9, 0.1,		0.9, 0.4,		0.8, 0.5},
	  {0.1, 0.1,		0.9, 0.1,		0.9, 0.4,		0.8, 0.5,		0.1, 0.4} },// b
	{ {0.9, 0.5,		0.2, 0.5,		0.1, 0.4,		0.1, 0.2,		0.2, 0.1},
	  {0.2, 0.5,		0.1, 0.4,		0.1, 0.2,		0.2, 0.1,		0.9, 0.1} },// c
	{ {0.9, 0.95,		0.9, 0.2,		0.1, 0.1,		0.1, 0.4,		0.2, 0.5},
	  {0.9, 0.1,		0.1, 0.1,		0.1, 0.4,		0.2, 0.5,		0.9, 0.4} },// d
	{ {0.9, 0.1,		0.1, 0.3,		0.2, 0.5,		0.8, 0.5,		0.9, 0.3},
	  {0.1, 0.3,		0.2, 0.5,		0.8, 0.5,		0.9, 0.3,		0.1, 0.3} },// e
	{ {0.5, 0.1,		0.5, 0.75,		0.6, 0.95,		0.3, 0.3,		0, 0},
	  {0.5, 0.75,		0.6, 0.95,		0.7, 0.95,		0.7, 0.3,		0, 0} },	// f
	{ {0.9, 0.9,		0.9, 0.1,		0.9, 0.8,		0.1, 0.9,		0.1, 0.5},
	  {0.9, 0.1,		0.1, 0.1,		0.1, 0.9,		0.1, 0.5,		0.9, 0.6} },// g
	{ {0.1, 0.9,		0.9, 0.1,		0.9, 0.4,		0.7, 0.5,		0.3, 0.5},
	  {0.1, 0.1,		0.9, 0.4,		0.7, 0.5,		0.3, 0.5,		0.1, 0.4} },// h
	{ {0.5, 0.1,		0.5, 0.5,		0.4, 0.1,		0.5, 0.6,		0, 0},
	  {0.5, 0.5,		0.4, 0.5,		0.6, 0.1,		0.5, 0.7,		0, 0} },	// i
	{ {0.5, 0.9,		0.4, 0.7,		0.5, 0.7,		0.5, 0.2,		0, 0},
	  {0.5, 0.8,		0.5, 0.7,		0.5, 0.2,		0.4, 0.1,		0, 0} },	// j
	{ {0.1, 0.9,		0.9, 0.1,		0.1, 0.3,		0, 0,			0, 0},
	  {0.1, 0.1,		0.1, 0.3,		0.9, 0.5,		0, 0,			0, 0} },	// k
	{ {0.5, 0.9,		0.4, 0.1,		0.5, 0.9,		0, 0,			0, 0},
	  {0.5, 0.1,		0.65, 0.1,		0.4, 0.9,		0, 0,			0, 0} },	// l
	{ {0.1, 0.1,		0.1, 0.5,		0.9, 0.5,		0.5, 0.5,		0, 0},
	  {0.1, 0.5,		0.9, 0.5,		0.9, 0.1,		0.5, 0.1,		0, 0} },	// m
	{ {0.1, 0.1,		0.1, 0.4,		0.9, 0.5,		0, 0,			0, 0},
	  {0.1, 0.5,		0.9, 0.5,		0.9, 0.1,		0, 0,			0, 0} },	// n
	{ {0.1, 0.1,		0.9, 0.1,		0.9, 0.5,		0.1, 0.5,		0, 0},
	  {0.9, 0.1,		0.9, 0.5,		0.1, 0.5,		0.1, 0.1,		0, 0} },	// o
	{ {0.1, 0.1,		0.15, 0.4,		0.8, 0.4,		0.9, 0.5,		0.8, 0.7},
	  {0.2, 0.7,		0.8, 0.4,		0.9, 0.5,		0.8, 0.7,		0.2, 0.7} },// p
	{ {0.9, 0.9,		0.9, 0.8,		0.2, 0.9,		0.1, 0.8,		0.1, 0.5},
	  {0.9, 0.1,		0.2, 0.9,		0.1, 0.8,		0.1, 0.5,		0.9, 0.6} },// q
	{ {0.1, 0.1,		0.1, 0.4,		0.2, 0.5,		0.8, 0.5,		0, 0},
	  {0.1, 0.5,		0.2, 0.5,		0.8, 0.5,		0.9, 0.4,		0, 0} },	// r
	{ {0.9, 0.5,		0.1, 0.45,		0.15, 0.3,		0.85, 0.3,		0.9, 0.15},
	  {0.1, 0.45,		0.15, 0.3,		0.85, 0.3,		0.9, 0.15,		0.1, 0.1} },// s
	{ {0.5, 0.9,		0.5, 0.15,		0.3, 0.7,		0, 0,			0, 0},
	  {0.5, 0.15,		0.6, 0.05,		0.7, 0.7,		0, 0,			0, 0} },	// t
	{ {0.1, 0.5,		0.1, 0.1,		0.9, 0.5,		0, 0,			0, 0},
	  {0.1, 0.1,		0.9, 0.2,		0.9, 0.1,		0, 0,			0, 0} },	// u
	{ {0.1, 0.5,		0.5, 0.1,		0, 0,			0, 0,			0, 0},
	  {0.5, 0.1,		0.9, 0.5,		0, 0,			0, 0,			0, 0} },	// v
	{ {0.1, 0.5,		0.1, 0.1,		0.9, 0.1,		0.5, 0.1,		0, 0},
	  {0.1, 0.1,		0.9, 0.1,		0.9, 0.5,		0.5, 0.5,		0, 0} },	// w
	{ {0.1, 0.5,		0.9, 0.5,		0, 0,			0, 0,			0, 0},
	  {0.9, 0.1,		0.1, 0.1,		0, 0,			0, 0,			0, 0} },	// x
	{ {0.9, 0.9,		0.9, 0.1,		0.1, 0.9,		0.1, 0.5,		0, 0},
	  {0.9, 0.1,		0.1, 0.1,		0.1, 0.5,		0.9, 0.6,		0, 0} },	// y
	{ {0.1, 0.5,		0.9, 0.5,		0.1, 0.1,		0, 0,			0, 0},
	  {0.9, 0.5,		0.1, 0.1,		0.9, 0.1,		0, 0,			0, 0} },	// z
	{ {0.3, 0.9,		0.2, 0.8,		0.2, 0.55,		0.1, 0.45,		0.2, 0.45},
	  {0.2, 0.8,		0.2, 0.55,		0.1, 0.45,		0.2, 0.45,		0.3, 0.1} },// {
	{ {0.5, 0.05,		0, 0,			0, 0,			0, 0,			0, 0},
	  {0.5, 0.95,		0, 0,			0, 0,			0, 0,			0, 0} },	// |
	{ {0.7, 0.9,		0.8, 0.8,		0.8, 0.55,		0.9, 0.45,		0.8, 0.45},
	  {0.8, 0.8,		0.8, 0.55,		0.9, 0.45,		0.8, 0.45,		0.7, 0.1} },// }
	{ {0.1, 0.4,		0.3, 0.6,		0.7, 0.4,		0, 0,			0, 0},
	  {0.3, 0.6,		0.7, 0.4,		0.9, 0.6,		0, 0,			0, 0} },	// ~
	  
};

void draw_line(Image *img, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;

	while (x1 != x2 || y1 != y2) {

		if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
			img->pixels[y1][x1][0] = r;
			img->pixels[y1][x1][1] = g;
			img->pixels[y1][x1][2] = b;
		}

		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; x1 += sx; }
		if (e2 < dx) { err += dx; y1 += sy; }
	}
}

void draw_char(Image *img, char c, int x1, int y1, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
	if(c < 32 || c > 126) return;
	int index = c - 32;
	for(int i = 0; i < MAX_STROKES; ++i) {
		int stroke_sx = x1 + width*font[index].startPos[2*i], stroke_sy = y1 + height*(1-font[index].startPos[2*i+1]);
		int stroke_ex = x1 + width*font[index].endPos[2*i], stroke_ey = y1 + height*(1-font[index].endPos[2*i+1]);
		draw_line(img, stroke_sx, stroke_sy, stroke_ex, stroke_ey, r, g, b);
	}
}

// Draw a string using draw_char
void draw_string(Image *img, const char *str, int x, int y, int width, int height, int spacing, uint8_t r, uint8_t g, uint8_t b) {
	int length = strlen(str);
	if (length == 0) return;
	int char_width = (width - (length - 1) * spacing) / length;
	int cursor_x = x;
	
	for (const char *c = str; *c; c++) {
		draw_char(img, *c, cursor_x, y, char_width, height, r, g, b);
		cursor_x += char_width + spacing;
	}
}

// Draw a string with line breaks
void draw_breaking_string(Image *img, const char *str, int x, int y, int width, int height, int spacing, int line_spacing, uint8_t r, uint8_t g, uint8_t b) {
	int line_count = 1, max_line = 0, chars_in_line = 0;
	for (const char *c = str; *c; c++) {
		if (*c == '\n') {
			++line_count;
			if(chars_in_line > max_line) max_line = chars_in_line;
			chars_in_line = 0;
		} else {
			++chars_in_line;
		}
	}
	
	int line_height = (height - (line_count - 1) * line_spacing) / line_count;
	int char_width = (width - (max_line - 1) * spacing) / max_line;
	int cursor_y = y, cursor_x = x;
	
		for (const char *c = str; *c; c++) {
		if (*c == '\n') {
			cursor_y += line_height + spacing;
			cursor_x = x;
			continue;
		}
		draw_char(img, *c, cursor_x, cursor_y, char_width, line_height, r, g, b);
		cursor_x += char_width + spacing;
	}
}

void draw_ratio_breaking_string(Image *img, const char *str, int x, int y, int width, int height, int spacing, int line_spacing, float char_ratio, uint8_t r, uint8_t g, uint8_t b) {
	int str_len = strlen(str);
	if (str_len == 0) return;

	// Step 0: Find the longest line in the input text
	int max_line_length = 0, current_line_length = 0;
	for (const char *c = str; *c; c++) {
		if (*c == '\n') {
			if (current_line_length > max_line_length) max_line_length = current_line_length;
			current_line_length = 0;
		} else {
			current_line_length++;
		}
	}
	if (current_line_length > max_line_length) max_line_length = current_line_length;

	int line_count;
	for(int i = 0; i < 24; ++i) { // Prevent infinite loops
		// Step 1: Compute total number of lines
		int chars_in_line = 0;
		line_count = 1;
		for (const char *c = str; *c; c++) {
			if (*c == '\n' || chars_in_line >= max_line_length) {
				line_count++;
				chars_in_line = 0;
				if (*c == '\n') continue;
			}
			chars_in_line++;
		}

		// Step 2: Compute effective width-to-height ratio
		float effective_ratio = ((float)line_count) * ((float)width) / ((float)max_line_length) / ((float)height);

		// Step 3: Adjust max line length
		max_line_length *= sqrt(effective_ratio/char_ratio);
	}
	
	if((float)width / char_ratio < (float)height) {
		y += height - (int)((float)width / char_ratio);
	}

	// Compute final character width & height
	int char_height = (height - (line_count-1)*line_spacing) / line_count;
	int char_width = ((float)char_height) * char_ratio;
	if (char_height < 5) char_height = 1;
	if (char_width < 1) char_width = 1;

	// Render the text
	int cursor_x = x, cursor_y = y;
	int chars_in_line = 0;

	for (const char *c = str; *c; c++) {
		if (*c == '\n' || chars_in_line >= max_line_length) {
			cursor_y += char_height + line_spacing;
			cursor_x = x;
			chars_in_line = 0;
			if (*c == '\n') continue;
		}

		draw_char(img, *c, cursor_x, cursor_y, char_width, char_height, r, g, b);
		cursor_x += char_width + spacing;
		chars_in_line++;
	}
}

void render_card(Image *img, Entry entry) {
	// Define card dimensions
	int card_w = WIDTH, card_h = HEIGHT;
	int outer_thickness = 8;
	int border_thickness = 8;
	int line_height = card_h / 16;
	int art_area_h = card_h / 2;

	// Determine border color
	uint8_t r = 128, g = 128, b = 128; // Default to gray (colorless)
	int color_count = 0;
	
	if (strchr(entry.cost, 'W')) { r = 255; g = 255; b = 200; color_count++; }
	if (strchr(entry.cost, 'U')) { r = 100; g = 100; b = 255; color_count++; }
	if (strchr(entry.cost, 'B')) { r = 80; g = 80; b = 80; color_count++; }
	if (strchr(entry.cost, 'R')) { r = 255; g = 80; b = 80; color_count++; }
	if (strchr(entry.cost, 'G')) { r = 80; g = 200; b = 80; color_count++; }

	// If multicolored, use gold border
	if (color_count > 1) { r = 218; g = 165; b = 32; }

	// Initialize the image with a black background
	init_image(img, 0, 0, 0);

	// Draw border
	draw_rect(img, outer_thickness, outer_thickness, card_w-outer_thickness, card_h-outer_thickness, r, g, b);
//	draw_rect(img, border_thickness, border_thickness, 
//		card_w - border_thickness, card_h - border_thickness, 255, 255, 255);

	// Draw placeholder for art
	draw_rect(img, outer_thickness+border_thickness, outer_thickness+border_thickness+line_height,
			  card_w - outer_thickness - border_thickness, outer_thickness+border_thickness+line_height+art_area_h, 0, 0, 0);

	// Draw name
	draw_string(img, entry.name, outer_thickness+border_thickness, outer_thickness+border_thickness, 
				card_w - 2*outer_thickness - 2*border_thickness, line_height, 0, 0, 0, 0);

	// Draw type line
	draw_string(img, entry.type, outer_thickness+border_thickness, outer_thickness+border_thickness+line_height+art_area_h, 
				card_w - 2*outer_thickness - 2*border_thickness, line_height, 0, 0, 0, 0);

	// Draw text box
	draw_rect(img, outer_thickness+border_thickness, outer_thickness+border_thickness+2*line_height+art_area_h,
			  card_w - outer_thickness - border_thickness, card_h-outer_thickness, 240, 240, 240);

	// Draw card text
	draw_ratio_breaking_string(img, entry.text, outer_thickness+border_thickness, outer_thickness+border_thickness+2*line_height+art_area_h, 
						 card_w - 2*outer_thickness - 2*border_thickness, card_h-2*outer_thickness-border_thickness-2*line_height-art_area_h, 0, 0, 0.6, 0, 0, 0);
}

void draw_linew(Image *img, int x1, int y1, int x2, int y2, int width, uint8_t r, uint8_t g, uint8_t b) {
	if (width < 1) width = 1;

	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;

	// Determine perpendicular offsets based on direction
	int wx = (dy == 0) ? 0 : (width / 2) * sy;  // Vertical thickness
	int wy = (dx == 0) ? 0 : (width / 2) * sx;  // Horizontal thickness

	while (x1 != x2 || y1 != y2) {
		for (int i = -width / 2; i <= width / 2; i++) {
			int px = x1 + (dy == 0 ? 0 : i);  // Adjust width along perpendicular
			int py = y1 + (dx == 0 ? 0 : i);

			if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
				img->pixels[py][px][0] = r;
				img->pixels[py][px][1] = g;
				img->pixels[py][px][2] = b;
			}
		}

		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; x1 += sx; }
		if (e2 < dx) { err += dx; y1 += sy; }
	}
}

#endif // CARD_RENDERER_H
