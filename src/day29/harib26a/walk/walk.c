#include "apilib.h"

void HariMain(void) {
	char* buf;
	int win, i, x, y;
	api_initmalloc();
	buf = api_malloc(160 * 100);
	win = api_openwin(buf, 160, 100, -1, "walk");
	api_boxfilwin(win, 4, 24, 155, 95, 0 /* 黑色 */);
	x = 76;
	y = 56;

	while (1) {
		api_putstrwin(win, x, y, 3 /* 黄色 */, 1, "*");
		i = api_getkey(1);
		api_putstrwin(win, x, y, 0 /* 黑色 */, 1, "*");		/* 用黑色擦除 */
		if (i == '4' && x > 4) {
			x -= 8;
		} else if (i == '6' && x < 148) {
			x += 8;
		} else if (i == '8' && y > 24) {
			y -= 8;
		} else if (i == '2' && y < 80) {
			y += 8;
		} else if (i == 0x0a) {		/* 按回车键结束 */
			break;
		}
	}
	api_closewin(win);
	api_end();
}
