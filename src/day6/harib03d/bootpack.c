#include <stdio.h>
#include "asmhead.h"
#include "graphic.h"
#include "naskfunc.h"

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	char s[40], mcursor[16 * 16];
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	while (1)
		io_hlt();
}
