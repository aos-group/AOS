#include <stdio.h>
#include "asmhead.h"
#include "graphic.h"
#include "naskfunc.h"
#include "int.h"
#include "dsctbl.h"
#include "fifo.h"
#include "kbdms.h"
#include "memory.h"

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	char s[40], mcursor[16 * 16], keybuf[32], mousebuf[128];
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;

	init_gdtidt();
	init_pic();
	io_sti();
	fifo8_init(&keyfifo, 32, keybuf);
	fifo8_init(&mousefifo, 128, mousebuf);
	io_out8(PIC0_IMR, 0xf9);	/* 1111 1001 接收键盘和 PIC1 的中断 */
	io_out8(PIC1_IMR, 0xef);	/* 1110 1111 接收鼠标的中断 */

	init_keyboard();
	enable_mouse(&mdec);
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);


	sprintf(s, "memory %dMB  free: %dKB", memtotal >> 20, memman_total(memman) >> 10);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	while (1) {
		io_cli();
		if (fifo8_status(&keyfifo) == 0 && fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
			} else if (fifo8_status(&mousefifo) != 0) {
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
					/* 鼠标的三个字节都齐了，显示出来 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0)
						s[1] = 'L';
					if ((mdec.btn & 0x02) != 0)
						s[3] = 'R';
					if ((mdec.btn & 0x04) != 0)
						s[2] = 'C';
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);

					/* 移动鼠标指针 */
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);		/* 隐藏鼠标 */
					mx += mdec.x / 5;
					my += mdec.y / 5;
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 16)
						mx = binfo->scrnx - 16;
					if (my > binfo->scrny - 16)
						my = binfo->scrny - 16;
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);		/* 隐藏坐标 */
					putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);		/* 显示坐标 */
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);	/* 描画鼠标 */
				}
			}
		}
	}
}
