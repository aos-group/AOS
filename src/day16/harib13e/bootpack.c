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
#include "timer.h"
#include "mtask.h"

extern struct TIMERCTL timerctl;

void task_b_main(struct SHEET* sht_win_b);

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	struct FIFO32 fifo;
	char s[40];
	int fifobuf[128];
	struct TIMER* timer;
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i, cursor_x, cursor_c;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct SHTCTL* shtctl;
	struct SHEET* sht_back, *sht_mouse, *sht_win, *sht_win_b[3];
	unsigned char* buf_back, buf_mouse[256], *buf_win, *buf_win_b;
	static char keytable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};
	struct TASK* task_a, *task_b[3];

	init_gdtidt();
	init_pic();
	io_sti();
	fifo32_init(&fifo, 128, fifobuf, 0);	/* 现在这里用 0 代替，也就是禁用自动唤醒功能 */
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8);	/* 1111 1000 接收键盘、 PIC1 和时钟的中断 */
	io_out8(PIC1_IMR, 0xef);	/* 1110 1111 接收鼠标的中断 */

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;		/* 从这里开始可以使用自动唤醒功能 */
	task_run(task_a, 1, 2);

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char*)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	/* 没有透明色 */
	init_screen(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_win_b */
	for (i = 0; i < 3; i++) {
		sht_win_b[i] = sheet_alloc(shtctl);
		buf_win_b = (unsigned char*)memman_alloc_4k(memman, 144 * 52);
		sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52, -1);		/* 没有透明色 */
		sprintf(s, "task_b%d", i);
		make_window8(buf_win_b, 144, 52, s, 0);
		task_b[i] = task_alloc();
		task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int)&task_b_main;
		task_b[i]->tss.es = 1 << 3;
		task_b[i]->tss.cs = 2 << 3;
		task_b[i]->tss.ss = 1 << 3;
		task_b[i]->tss.ds = 1 << 3;
		task_b[i]->tss.fs = 1 << 3;
		task_b[i]->tss.gs = 1 << 3;
		*((int*)(task_b[i]->tss.esp + 4)) = (int)sht_win_b[i];
		task_run(task_b[i], 2, i + 1);
	}

	/* sht_win */
	sht_win = sheet_alloc(shtctl);
	buf_win = (unsigned char*)memman_alloc_4k(memman, 144 * 52);
	sheet_setbuf(sht_win, buf_win, 144, 52, -1);
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);		/* 透明色号 99 */
	init_mouse_cursor8(buf_mouse, 99);		/* 背景色号 99 */
	
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_win_b[0], 168, 56);
	sheet_slide(sht_win_b[1], 8, 116);
	sheet_slide(sht_win_b[2], 168, 116);
	sheet_slide(sht_win, 8, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_win_b[0], 1);
	sheet_updown(sht_win_b[1], 2);
	sheet_updown(sht_win_b[2], 3);
	sheet_updown(sht_win, 4);
	sheet_updown(sht_mouse, 5);

	sprintf(s, "(%4d, %4d)", mx, my);
	putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 12);
	sprintf(s, "memory: %dMB  free: %dKB", memtotal >> 20, memman_total(memman) >> 10);
	putfonts8_asc_sht(sht_back, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);

	while (1) {
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			task_sleep(task_a);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (256 <= i && i <= 511) {		/* 键盘数据 */
				sprintf(s, "%02X", i - 256);
				putfonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				if (i < 0x54 + 256) {
					if (keytable[i - 256] && cursor_x < 144) {	/* 一般字符 */
						s[0] = keytable[i - 256];
						s[1] = 0;
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
						cursor_x += 8;
					} else if (i == 0x0e + 256 && cursor_x > 8)	{	/* 退格键 */
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
						cursor_x -= 8;
					}
					/* 光标再显示 */
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				}
			} else if (512 <= i && i <= 767) {	/* 鼠标数据 */
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* 鼠标的三个字节都齐了，显示出来 */
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0)
						s[1] = 'L';
					if ((mdec.btn & 0x02) != 0)
						s[3] = 'R';
					if ((mdec.btn & 0x04) != 0)
						s[2] = 'C';
					putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);

					/* 移动鼠标指针 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) mx = 0;
					if (my < 0) my = 0;
					if (mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1) my = binfo->scrny - 1;
					sprintf(s, "(%4d, %4d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 12);
					sheet_slide(sht_mouse, mx, my);										/* 描画鼠标 */
					if (mdec.btn & 0x01) {	/* 按下左键，移动sht_win */
						sheet_slide(sht_win, mx - 80, my - 8);
					}
				}
			} else if (i <= 1) {	/* 光标用定时器 */
				if (i) {
					timer_init(timer, &fifo, 0);
					cursor_c = COL8_000000;
				} else {
					timer_init(timer, &fifo, 1);
					cursor_c = COL8_FFFFFF;
				}
				timer_settime(timer, 50);
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			}
		}
	}
}

void task_b_main(struct SHEET* sht_win_b) {
	struct FIFO32 fifo;
	struct TIMER* timer_ls;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];

	fifo32_init(&fifo, 128, fifobuf, 0);
	timer_ls = timer_alloc();
	timer_init(timer_ls, &fifo, 100);
	timer_settime(timer_ls, 100);

	while (1) {
		count++;
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (i == 100) {
				sprintf(s, "%11d", count - count0);
				putfonts8_asc_sht(sht_win_b, 24, 28, COL8_000000, COL8_C6C6C6, s, 11);
				count0 = count;
				timer_settime(timer_ls, 100);
			}
		}
	}
}
