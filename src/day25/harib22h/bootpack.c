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
#include "window.h"
#include "console.h"

extern struct TIMERCTL timerctl;

struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;

int keywin_off(struct SHEET* key_win, struct SHEET* sht_win, int cur_c, int cur_x);
int keywin_on(struct SHEET* key_win, struct SHEET* sht_win, int cur_c);

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct FIFO32 fifo, keycmd;
	char s[40];
	int fifobuf[128], keycmdbuf[32];
	struct TIMER* timer;
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i, cursor_x, cursor_c;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct SHTCTL* shtctl;
	struct SHEET* sht_back, *sht_mouse, *sht_win, *sht_cons[2];
	unsigned char* buf_back, buf_mouse[256], *buf_win, *buf_cons[2];
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
	struct TASK* task_a, *task_cons[2], *task;
	int key_shift = 0;
	int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y;
	int mmx = -1, mmy = -1;		/* 窗口移动之前的坐标，为负数时代表当前不处于窗口移动模式 */
	struct SHEET* sht = 0;
	struct SHEET* key_win;		/* 当前处于输入模式的窗口地址 */

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
	*((int*)0x0fe4) = (int)shtctl;
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char*)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	/* 没有透明色 */
	init_screen(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	for (i = 0; i < 2; i++) {
		sht_cons[i] = sheet_alloc(shtctl);
		buf_cons[i] = (unsigned char*)memman_alloc_4k(memman, 256 * 165);
		sheet_setbuf(sht_cons[i], buf_cons[i], 256, 165, -1);		/* 没有透明色 */
		make_window8(buf_cons[i], 256, 165, "console", 0);
		make_textbox8(sht_cons[i], 8, 28, 240, 128, COL8_000000);
		task_cons[i] = task_alloc();
		task_cons[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
		task_cons[i]->tss.eip = (int)&console_task;
		task_cons[i]->tss.es = 1 << 3;
		task_cons[i]->tss.cs = 2 << 3;
		task_cons[i]->tss.ss = 1 << 3;
		task_cons[i]->tss.ds = 1 << 3;
		task_cons[i]->tss.fs = 1 << 3;
		task_cons[i]->tss.gs = 1 << 3;
		*((int*)(task_cons[i]->tss.esp + 4)) = (int)sht_cons[i];
		*((int*)(task_cons[i]->tss.esp + 8)) = memtotal;
		task_run(task_cons[i], 2, 2);
		sht_cons[i]->task = task_cons[i];
		sht_cons[i]->flags |= SHEET_CURSOR;		/* 有光标 */
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
	sheet_slide(sht_cons[1], 56, 6);
	sheet_slide(sht_cons[0], 8, 2);
	sheet_slide(sht_win, 64, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_cons[1], 1);
	sheet_updown(sht_cons[0], 2);
	sheet_updown(sht_win, 3);
	sheet_updown(sht_mouse, 4);
	key_win = sht_win;

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
			if (key_win->flags == SHEET_UNUSE) {	/* 输入窗口被关闭 */
				key_win = shtctl->sheets[shtctl->top - 1];		/* 让系统自动切换到最上层的窗口 */
				cursor_c = keywin_on(key_win, sht_win, cursor_c);
			}
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
					if (key_win == sht_win) {	/* 发送给任务 A */
						if (cursor_x < 128) {
							s[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursor_x += 8;
						}
					} else {	/* 发送给命令行窗口 */
						fifo32_put(&key_win->task->fifo, s[0] + 256);
					}
				} else if (i == 0x0e + 256)	{	/* 退格键 */
					if (key_win == sht_win) {	/* 发送给任务 A */
						if (cursor_x > 8) {
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
							cursor_x -= 8;
						}
					} else {	/* 发送给命令行窗口 */
						fifo32_put(&key_win->task->fifo, 8 + 256);	/* ASCII 8 对应退格键 */
					}
				} else if (i == 0x0f + 256) {	/* Tab 键 */
					cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					cursor_c = keywin_on(key_win, sht_win, cursor_c);
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
					if (key_win != sht_win)
						fifo32_put(&key_win->task->fifo, 10 + 256);		/* ASCII 10 对应回车键 */
				} else if (i == 0x3b + 256 && key_shift != 0) {		/* Shift + F1, 强制关闭窗口 */
					task = key_win->task;
					if (task && task->tss.ss0 != 0) {
						cons_putstr0(task->cons, "\nBreak(key):\n");
						io_cli();	/* 不能在改变寄存器时切换到其他任务 */
						task->tss.eax = (int)&(task->tss.esp0);
						task->tss.eip = (int)asm_end_app;
						io_sti();
					}
				} else if (i == 0x57 + 256 && shtctl->top > 2) {	/* F11, 将最下面窗口放到最上面 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
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
					if (mdec.btn & 0x01) {		/* 按下左键 */
						if (mmx < 0) {
							/* 如果处于通常模式 */
							/* 按照从上到下的顺序寻找鼠标所指向的图层 */
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + y] != sht->col_inv) {
										sheet_updown(sht, shtctl->top - 1);
										if (sht != key_win) {
											cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
											key_win = sht;
											cursor_c = keywin_on(key_win, sht_win, cursor_c);
										}
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											/* 进入窗口移动模式 */
											mmx = mx;
											mmy = my;
										}
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
											/* 点击 "x" 按钮 */
											if (sht->flags & SHEET_FROM_APPLICATION) {	/* 该窗口是否为应用程序窗口? */
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse):\n");
												io_cli();	/* 不能在改变寄存器时切换到其他任务 */
												task->tss.eax = (int)&(task->tss.esp0);
												task->tss.eip = (int)asm_end_app;
												io_sti();
											}
										}
										break;
									}
								}
							}
						} else {
							/* 如果处于窗口移动模式 */
							x = mx - mmx;
							y = my - mmy;
							sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							mmx = mx;
							mmy = my;
						}
					} else {
						/* 没有按下左键 */
						mmx = -1;
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

/**
 * keywin_off - 失去焦点后，控制窗口(@key_win)标题栏的颜色和 task_a 窗口(@sht_win)的光标
 * @cur_c: task_a 窗口光标颜色
 * @cur_x: task_a 窗口光标横坐标
 * @return(cur_c): 修改后的 task_a 窗口光标颜色
 */
int keywin_off(struct SHEET* key_win, struct SHEET* sht_win, int cur_c, int cur_x) {
	change_wtitle8(key_win, 0);
	if (key_win == sht_win) {
		cur_c = -1;		/* 删除光标 */
		boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cur_x, 28, cur_x + 7, 43);
	} else {
		if (key_win->flags & SHEET_CURSOR)
			fifo32_put(&key_win->task->fifo, 3);	/* 命令行窗口光标 OFF */
	}
	return cur_c;
}

/**
 * keywin_off - 获取焦点后，控制窗口(@key_win)标题栏的颜色和 task_a 窗口(@sht_win)的光标
 * @cur_c: task_a 窗口光标颜色
 * @return(cur_c): 修改后的 task_a 窗口光标颜色
 */
int keywin_on(struct SHEET* key_win, struct SHEET* sht_win, int cur_c) {
	change_wtitle8(key_win, 1);
	if (key_win == sht_win) {
		cur_c = COL8_000000;
	} else {
		if (key_win->flags & SHEET_CURSOR)
			fifo32_put(&key_win->task->fifo, 2);	/* 命令行窗口光标 ON */
	}
	return cur_c;
}
