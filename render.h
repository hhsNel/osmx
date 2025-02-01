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



#endif // CARD_RENDERER_H
