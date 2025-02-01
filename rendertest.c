#include "render.h"

int main(int argc, char **argv) {
	Image img;
	init_image(&img, 0, 0, 127);
	draw_rect(&img, 0, 32, 128, 64, 255, 0, 0);
	draw_line(&img, WIDTH-1, 0, 0, HEIGHT-1, 0, 255, 0);
	
	draw_string(&img, " !\"#$%&'()*+,-./", 16, 256, WIDTH-32, 32, 0, 127, 127, 127);
	
	save_farbfeld("test.ff", &img);
}