#include <stdio.h>
#include <string.h>
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

#define KEYCMD_LED	0xed

extern struct TIMERCTL timerctl;

void console_task(struct SHEET* sheet, unsigned int memtotal);
int cons_newline(int cursor_y, struct SHEET* sheet);

struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	struct FIFO32 fifo, keycmd;
	char s[40];
	int fifobuf[128], keycmdbuf[32];
	struct TIMER* timer;
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i, cursor_x, cursor_c;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
	struct SHTCTL* shtctl;
	struct SHEET* sht_back, *sht_mouse, *sht_win, *sht_cons;
	unsigned char* buf_back, buf_mouse[256], *buf_win, *buf_cons;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	struct TASK* task_a, *task_cons;
	int key_to = 0;		/* 将字符写进哪个窗口 */
	int key_shift = 0;
	int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;

	init_gdtidt();
	init_pic();
	io_sti();
	fifo32_init(&fifo, 128, fifobuf, 0);
	fifo32_init(&keycmd, 32, keycmdbuf, 0);
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
	fifo.task = task_a;
	task_run(task_a, 1, 2);

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char*)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	/* 没有透明色 */
	init_screen(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	sht_cons = sheet_alloc(shtctl);
	buf_cons = (unsigned char*)memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -1);		/* 没有透明色 */
	make_window8(buf_cons, 256, 165, "console", 0);
	make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);
	task_cons = task_alloc();
	task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
	task_cons->tss.eip = (int)&console_task;
	task_cons->tss.es = 1 << 3;
	task_cons->tss.cs = 2 << 3;
	task_cons->tss.ss = 1 << 3;
	task_cons->tss.ds = 1 << 3;
	task_cons->tss.fs = 1 << 3;
	task_cons->tss.gs = 1 << 3;
	*((int*)(task_cons->tss.esp + 4)) = (int)sht_cons;
	*((int*)(task_cons->tss.esp + 8)) = memtotal;
	task_run(task_cons, 2, 2);

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
	sheet_slide(sht_cons, 100, 100);
	sheet_slide(sht_win, 64, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_cons, 1);
	sheet_updown(sht_win, 2);
	sheet_updown(sht_mouse, 3);

	/* 为了避免和键盘当前状态冲突，在一开始先进行设置 */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	while (1) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 如果存在向键盘控制器发送的数据，则发送它 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			task_sleep(task_a);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (256 <= i && i <= 511) {		/* 键盘数据 */
				if (i < 0x80 + 256) {	/* 将按键编码转化为字符编码 */
					if (key_shift) {
						s[0] = keytable1[i - 256];
					} else {
						s[0] = keytable0[i - 256];
					}
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') {
					if (((key_leds & 4) == 0 && key_shift == 0) ||
							((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* 将大写字母转换为小写字母 */
					}
				}
				if (s[0]) {	/* 一般字符 */
					if (key_to == 0) {	/* 发送给任务 A */
						if (cursor_x < 128) {
							s[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursor_x += 8;
						}
					} else {	/* 发送给命令行窗口 */
						fifo32_put(&task_cons->fifo, s[0] + 256);
					}
				} else if (i == 0x0e + 256)	{	/* 退格键 */
					if (key_to == 0) {	/* 发送给任务 A */
						if (cursor_x > 8) {
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
							cursor_x -= 8;
						}
					} else {	/* 发送给命令行窗口 */
						fifo32_put(&task_cons->fifo, 8 + 256);	/* ASCII 8 对应退格键 */
					}
				} else if (i == 0x0f + 256) {	/* Tab 键 */
					if (key_to == 0) {
						key_to = 1;
						make_wtitle8(buf_win, sht_win->bxsize, "task_a", 0);
						make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
						cursor_c = -1;	/* 不显示光标 */
						boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28, cursor_x + 7, 43);
						fifo32_put(&task_cons->fifo, 2);	/* 命令行窗口光标 ON */
					} else {
						key_to = 0;
						make_wtitle8(buf_win, sht_win->bxsize, "task_a", 1);
						make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
						cursor_c = COL8_000000;		/* 显示光标 */
						fifo32_put(&task_cons->fifo, 3);	/* 命令行窗口 OFF */
					}
					sheet_refresh(sht_win, 0, 0, sht_win->bxsize, 21);
					sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
				} else if (i == 0x2a + 256) {	/* 左 Shift ON */
					key_shift |= 1;
				} else if (i == 0x36 + 256) {	/* 右 Shift ON */
					key_shift |= 2;
				} else if (i == 0xaa + 256) {	/* 左 Shift OFF */
					key_shift &= ~1;
				} else if (i == 0xb6 + 256) {	/* 右 Shift OFF */
					key_shift &= ~2;
				} else if (i == 0x3a + 256) {	/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				} else if (i == 0x45 + 256) {	/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				} else if (i == 0x46 + 256) {	/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				} else if (i == 0xfa + 256) {	/* 键盘成功接收到数据 */
					keycmd_wait = -1;
				} else if (i == 0xfe + 256) {	/* 键盘没有成功接收到数据 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				} else if (i == 0x1c + 256) {	/* 回车键 */
					if (key_to)
						fifo32_put(&task_cons->fifo, 10 + 256);		/* ASCII 10 对应回车键 */
				}
				/* 光标再显示 */
				if (cursor_c >= 0)
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			} else if (512 <= i && i <= 767) {	/* 鼠标数据 */
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* 鼠标的三个字节都齐了 */
					/* 移动鼠标指针 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) mx = 0;
					if (my < 0) my = 0;
					if (mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1) my = binfo->scrny - 1;
					sheet_slide(sht_mouse, mx, my);										/* 描画鼠标 */
					if (mdec.btn & 0x01) {	/* 按下左键，移动sht_win */
						sheet_slide(sht_win, mx - 80, my - 8);
					}
				}
			} else if (i <= 1) {	/* 光标用定时器 */
				if (i) {
					timer_init(timer, &fifo, 0);
					if (cursor_c >= 0)
						cursor_c = COL8_000000;
				} else {
					timer_init(timer, &fifo, 1);
					if (cursor_c >= 0)
						cursor_c = COL8_FFFFFF;
				}
				timer_settime(timer, 50);
				if (cursor_c >= 0) {
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				}
			}
		}
	}
}

void console_task(struct SHEET* sheet, unsigned int memtotal) {
	struct TIMER* timer;
	struct TASK* task = task_now();
	int i, fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
	char s[30], cmdline[30], name[9], *p;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
	int x, y;
	struct FILEINFO* finfo = (struct FILEINFO*)(ADR_DISKIMG + 0x002600);

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);

	/* 显示提示符 */
	putfonts8_asc_sht(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);

	while (1) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) {	/* 光标用定时器 */
				if (i) {
					timer_init(timer, &task->fifo, 0);
					if (cursor_c >= 0)
						cursor_c = COL8_FFFFFF;
				} else {
					timer_init(timer, &task->fifo, 1);
					if (cursor_c >= 0)
						cursor_c = COL8_000000;
				}
				timer_settime(timer, 50);
			} else if (i == 2) {	/* 光标 ON */
				cursor_c = COL8_FFFFFF;
			} else if (i == 3) {	/* 光标 OFF */
				cursor_c = -1;
				boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			} else if (25 + 6 <= i && i < 512) {	/* 键盘数据(通过任务 A) */
				if (i == 8 + 256) {
					/* 退格键 */
					if (cursor_x > 16) {
						putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
						cursor_x -= 8;
					}
				} else if (i == 10 + 256) {
					/* 回车键 */
					putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1); 
					cmdline[(cursor_x - 16) >> 3] = 0;
					cursor_y = cons_newline(cursor_y, sheet);
					/* 执行命令 */
					if (strcmp(cmdline, "mem") == 0) {	/* mem 命令 */
						sprintf(s, "total %dMB", memtotal >> 20);
						putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						sprintf(s, "free %dKB", memman_total(memman) >> 10);
						putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (strcmp(cmdline, "cls") == 0) {	/* cls 命令 */
						for (y = 28; y < 28 + 112; y++) {
							for (x = 8; x < 8 + 240; x++) {
								sheet->buf[x + y * sheet->bxsize] = COL8_000000;
							}
						}
						sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					} else if (strcmp(cmdline, "dir") == 0) {	/* dir 命令 */
						for (x = 0; x < 224; x++) {
							if (finfo[x].name[0] == 0x00)	/* 不包含任何文件信息 */
								break;
							if (finfo[x].name[0] != 0xe5) {		/* 没有被删除 */
								if ((finfo[x].type & 0x18) == 0) {	/* 普通文件 */
									strncpy(name, finfo[x].name, 8);	/* 将文件名与后缀分开 */
									sprintf(s, "%s.%s %7d", name, finfo[x].ext, finfo[x].size);
									putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
									cursor_y = cons_newline(cursor_y, sheet);
								}
							}
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (strncmp(cmdline, "type ", 5) == 0) {		/* type 命令 */
						for (y = 0; y < 12; y++) {
							s[y] = ' ';
						}
						s[12] = 0;	/* finfo[x].name 长度为 12 字节 */
						y = 0;
						for (x = 5; y < 11 && cmdline[x]; x++) {
							if (cmdline[x] == '.' && y <= 8) {
								y = 8;
							} else {
								s[y] = cmdline[x];
								if ('a' <= s[y] && s[y] <= 'z') {
									s[y] -= 0x20;	/* 将小写字母转换成大写字母 */
								}
								y++;
							}
						}
						/* 寻找文件 */
						for (x = 0; x < 224; x++) {
							if (finfo[x].name[0] == 0x00 || ((finfo[x].type & 0x18) == 0 && strcmp(finfo[x].name, s) == 0)) {
								break;
							}
						}
						if (x < 224 && finfo[x].name[0] != 0x00) {	/* 找到文件 */
							y = finfo[x].size;
							p = (char*)(finfo[x].clustno * 512 + 0x003e00 + ADR_DISKIMG);
							cursor_x = 8;
							for (x = 0; x < y; x++) {	/* 逐字输出 */
								s[0] = p[x];
								s[1] = 0;
								if (s[0] == 0x09) {		/* 制表符 */
									do {
										putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
										cursor_x += 8;
										if (cursor_x == 8 + 240) {
											cursor_x = 8;
											cursor_y = cons_newline(cursor_y, sheet);
										}
									} while (((cursor_x - 8) & 0x1f) != 0);	/* 不被 32 整除 */
								} else if (s[0] == 0x0a) {	/* 换行 */
									cursor_x = 8;
									cursor_y = cons_newline(cursor_y, sheet);
								} else if (s[0] == 0x0d) {	/* 回车 */
									/* 这里暂且不进行任何操作 */
								} else {	/* 一般字符 */
									putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
									cursor_x += 8;
									if (cursor_x == 8 + 240) {
										cursor_x = 8;
										cursor_y = cons_newline(cursor_y, sheet);
									}
								}
							}
							cursor_y = cons_newline(cursor_y, sheet);
						} else {	/* 没有找到文件 */
							putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
							cursor_y = cons_newline(cursor_y, sheet);
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (cmdline[0] != 0) {
						/* 不是命令，也不是空行 */
						putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Bad command.", 30);
						cursor_y = cons_newline(cursor_y, sheet);
						cursor_y = cons_newline(cursor_y, sheet);
					}
					putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, ">", 1);
					cursor_x = 16;
				} else {
					/* 一般字符 */
					if (cursor_x < 240) {
						s[0] = i - 256;
						s[1] = 0;
						cmdline[(cursor_x - 16) >> 3] = i - 256;
						putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
						cursor_x += 8;
					}
				}
			}
			/* 光标再显示 */
			if (cursor_c >= 0)
				boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}
	}
}

int cons_newline(int cursor_y, struct SHEET* sheet) {
	int x, y;
	if (cursor_y < 28 + 112) {
		cursor_y += 16;
	} else {	/* 滚动 */
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	return cursor_y;
}
