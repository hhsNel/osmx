#define MAX_LINE 1024

typedef struct {
	char name[MAX_LINE];
	char cost[MAX_LINE];
	char type[MAX_LINE];
	char mainType[MAX_LINE];
	char text[MAX_LINE * 2];
	char power[MAX_LINE];
	char toughness[MAX_LINE];
	char loyalty[MAX_LINE];
} Entry;

#include "render.h"

int main(int argc, char **argv) {
	Image img;
	init_image(&img, 0, 0, 127);
	draw_rect(&img, 0, 32, 128, 64, 255, 0, 0);
	draw_line(&img, WIDTH-1, 0, 0, HEIGHT-1, 0, 255, 0);
	
	draw_breaking_string(&img, " !\"#$%&'()*+,-./\n0123456789:;<=>?\n@ABCDEFGHIJKLMNO\nPQRSTUVWXYZ[\\]^_\n`abcdefghijklmno\npqrstuvwxyz{|}~", 16, 256, WIDTH-32, 256, 0, 2, 127, 127, 127);
	
	draw_ratio_breaking_string(&img, "This is a test", 10, 10, 200, 100, 2, 5, 0.6, 255, 255, 255);

	
	save_farbfeld("test.ff", &img);
}