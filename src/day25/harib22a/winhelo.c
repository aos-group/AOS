#include "a_nask.h"

void HariMain(void) {
	char* buf;
	int win;

	api_initmalloc();
	buf = api_malloc(150 * 50);
	win = api_openwin(buf, 150, 50, -1, "hello");
	api_boxfilwin(win, 8, 36, 141, 43, 3 /* 黄色 */);
	api_putstrwin(win, 28, 28, 0 /* 黑色 */, 12, "hello, world");
	// api_free(buf, 150 * 50);
	api_end();
}
