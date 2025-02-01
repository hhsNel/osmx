#include "render.h"

int main(int argc, char **argv) {
	Image img;
	init_image(&img, 0, 0, 0);
	draw_rect(&img, 0, 32, 0, 64, 255, 0, 0);
	draw_line(&img, WIDTH-1, 0, HEIGHT-1, 0, 0, 255, 0);
	
}