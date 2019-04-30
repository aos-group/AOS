#include <stdio.h>
#include "asmhead.h"
#include "graphic.h"
#include "naskfunc.h"
#include "int.h"
#include "dsctbl.h"
#include "fifo.h"
#include "kbdms.h"
#include "memory.h"
#include "sheet.h"

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	char s[40], keybuf[32], mousebuf[128];
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct SHTCTL* shtctl;
	struct SHEET* sht_back, *sht_mouse, *sht_win;
	unsigned char* buf_back, buf_mouse[256], *buf_win;

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
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);
	sht_win = sheet_alloc(shtctl);
	buf_back = (unsigned char*)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win = (unsigned char*)memman_alloc_4k(memman, 160 * 68);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	/* 没有透明色 */
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);		/* 透明色号 99 */
	sheet_setbuf(sht_win, buf_win, 160, 68, -1);
	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	init_mouse_cursor8(buf_mouse, 99);		/* 背景色号 99 */
	make_window8(buf_win, 160, 68, "window");
	putfonts8_asc(buf_win, 160, 24, 28, COL8_000000, "Welcome to");
	putfonts8_asc(buf_win, 160, 24, 44, COL8_000000, " Haribote-OS!");
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_win, 80, 72);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_win, 1);
	sheet_updown(sht_mouse, 2);

	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	sprintf(s, "memory: %dMB  free: %dKB", memtotal >> 20, memman_total(memman) >> 10);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
	sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);

	while (1) {
		io_cli();
		if (fifo8_status(&keyfifo) == 0 && fifo8_status(&mousefifo) == 0) {
			io_stihlt();
		} else {
			if (fifo8_status(&keyfifo) != 0) {
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putfonts8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheet_refresh(sht_back, 0, 16, 16, 32);
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
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
					putfonts8_asc(buf_back, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
					sheet_refresh(sht_back, 32, 16, 32 + 15 * 8, 32);

					/* 移动鼠标指针 */
					mx += mdec.x / 3;
					my += mdec.y / 3;
					if (mx < 0)
						mx = 0;
					if (my < 0)
						my = 0;
					if (mx > binfo->scrnx - 1)
						mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1)
						my = binfo->scrny - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15);		/* 隐藏坐标 */
					putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);		/* 显示坐标 */
					sheet_refresh(sht_back, 0, 0, 80, 16);
					sheet_slide(sht_mouse, mx, my);										/* 描画鼠标 */
				}
			}
		}
	}
}
