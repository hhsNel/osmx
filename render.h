#ifndef CARD_RENDERER_H
#define CARD_RENDERER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
    fputc(0x66, f); fputc(0x61, f); fputc(0x72, f); fputc(0x62, f);
    fputc(0x66, f); fputc(0x65, f); fputc(0x6c, f); fputc(0x64, f);
    fputc((WIDTH >> 24) & 0xFF, f); fputc((WIDTH >> 16) & 0xFF, f);
    fputc((WIDTH >> 8) & 0xFF, f); fputc(WIDTH & 0xFF, f);
    fputc((HEIGHT >> 24) & 0xFF, f); fputc((HEIGHT >> 16) & 0xFF, f);
    fputc((HEIGHT >> 8) & 0xFF, f); fputc(HEIGHT & 0xFF, f);
    
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

void draw_char(Image *img, int x1, int y1, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
	
}

#endif // CARD_RENDERER_H
