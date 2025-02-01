#ifndef CARD_RENDERER_H
#define CARD_RENDERER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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

CharRender font[96] = {
	/* each character is 2 lines. The first line contains the starting positions,
	   the second contains ending positions. Each position contains 2 floating point
	   numbers, the X and Y. There are 5 positions in each line */
	/* 1st start/end	2nd start/end	3rd start/end	4th start/end	5th start/end */
	{ {0, 0,			0, 0,			0, 0,			0, 0,			0, 0}, 
	  {0, 0,			0, 0,			0, 0,			0, 0,			0, 0} },	// space
	{ {0.5, 0.9,		0.5, 0.2,		0, 0,			0, 0,			0, 0}, 
	  {0.5, 0.3,		0.5, 0.1,		0, 0,			0, 0,			0, 0} },	// !
	{ {0.4, 0.6,		0.6, 0.6,		0, 0,			0, 0,			0, 0}, 
	  {0.4, 0.4,		0.6, 0.4,		0, 0,			0, 0,			0, 0} },	// "
	{ {0.3, 0.1,		0.65, 0.1,		0.1, 0.3,		0.1, 0.7,		0, 0}, 
	  {0.35, 0.9,		0.7, 0.9,		0.9, 0.3,		0.9, 0.7,		0, 0} },	// #
	{ {0.1, 0.9,		0.1, 0.9,		0.1, 0.1,		0.5, 0.95,		0, 0}, 
	  {0.9, 0.9,		0.9, 0.1,		0.9, 0.9,		0.5, 0.05,		0, 0} },	// $
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
	if(c < 32 || c > 127) return;
	int index = c - 32;
	for(int i = 0; i < MAX_STROKES; ++i) {
		int stroke_sx = x1 + width*(font[index].startPos[2*i]), stroke_sy = y1 + height*(font[index].startPos[2*i+1]);
		int stroke_ex = x1 + width*(font[index].endPos[2*i]), stroke_ey = y1 + height*(font[index].endPos[2*i+1]);
		draw_line(img, stroke_sx, stroke_sy, stroke_ex, stroke_ey, r, g, b);
	}
}

#endif // CARD_RENDERER_H
