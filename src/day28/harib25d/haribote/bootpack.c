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
extern struct TASKCTL* taskctl;

struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;

void keywin_off(struct SHEET* key_win);
void keywin_on(struct SHEET* key_win);

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct FIFO32 fifo, keycmd;
	char s[40];
	int fifobuf[128], keycmdbuf[32];
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;
	int i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	struct MOUSE_DEC mdec;
	unsigned int memtotal;
	struct SHTCTL* shtctl;
	struct SHEET* sht_back, *sht_mouse;
	unsigned char* buf_back, buf_mouse[256];
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08,  0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	struct TASK* task_a, *task;
	int key_shift = 0;
	int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y;
	int mmx = -1, mmy = -1;		/* 窗口移动之前的坐标，为负数时代表当前不处于窗口移动模式 */
	int mmx2 = 0;	/* 保存移动前的 sht->vx0 的值 */
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
	key_win = open_console(shtctl, memtotal);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);		/* 透明色号 99 */
	init_mouse_cursor8(buf_mouse, 99);		/* 背景色号 99 */
	
	sheet_slide(sht_back, 0, 0);
	sheet_slide(key_win, 32, 4);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(key_win, 1);
	sheet_updown(sht_mouse, 2);
	keywin_on(key_win);

	/* 为了避免和键盘当前状态冲突，在一开始先进行设置 */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	*((int*)0x0fec) = (int)&fifo;

	while (1) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 如果存在向键盘控制器发送的数据，则发送它 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			/* FIFO 为空，当存在搁置的绘图操作时立即执行 */
			if (new_mx >= 0) {		/* 鼠标的坐标不可能为负数 */
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {		/* 窗口的坐标有可能为负数 */
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_a);
				io_sti();
			}
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win && key_win->flags == SHEET_UNUSE) {	/* 输入窗口被关闭 */
				if (shtctl->top == 1) {		/* 当画面上只剩鼠标和背景时 */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];		/* 让系统自动切换到最上层的窗口 */
					keywin_on(key_win);
				}
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
				if (s[0] && key_win) {	/* 一般字符、 退格键、 回车键 */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				} else if (i == 0x0f + 256 && key_win) {	/* Tab 键 */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
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
				} else if (i == 0x3b + 256 && key_shift != 0 && key_win) {		/* Shift + F1, 强制关闭窗口 */
					task = key_win->task;	/* 有可能图层所在的任务已经结束了 */
					if (task && task->tss.ss0 != 0) {
						cons_putstr0(task->cons, "\nBreak(key):\n");
						io_cli();	/* 不能在改变寄存器时切换到其他任务 */
						task->tss.eax = (int)&(task->tss.esp0);
						task->tss.eip = (int)asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				} else if (i == 0x3c + 256 && key_shift != 0) {		/* Shift + F2, 打开新的命令行窗口 */
					/* 自动将输入焦点切换到新打开的命令行窗口 */
					if (key_win) 
						keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				} else if (i == 0x57 + 256 && shtctl->top > 2) {	/* F11, 将最下面窗口放到最上面 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}
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
					new_mx = mx;
					new_my = my;
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
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											/* 进入窗口移动模式 */
											mmx = mx;
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
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
												task_run(task, -1, 0);
											} else {	/* 命令行窗口 */
												task = sht->task;
												sheet_updown(sht, -1);	/* 暂且隐藏该图层 */
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												fifo32_put(&task->fifo, 4);
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
							new_wx = (mmx2 + x + 2) & ~3;	/* 四舍五入 */
							new_wy += y;
							mmy = my;
						}
					} else {
						/* 没有按下左键 */
						mmx = -1;
						if (new_wx != 0x7fffffff) {
							/* 即使 FIFO 不为空也需要立即更新窗口的位置，因为用户可能马上会去移动别的窗口，那样的话 sht 变量的值就会发生变化 */
							sheet_slide(sht, new_wx, new_wy);
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i <= 1023) {		/* 命令行窗口(显示)结束处理 */
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {	/* 命令行窗口(不显示)结束处理 */
				close_constask(taskctl->tasks0 + i - 1024);
			} else if (2024 <= i && i <= 2279) {	/* 只关闭命令行窗口 */
				close_console_only(shtctl->sheets0 + (i - 2024));
			}
		}
	}
}

/**
 * keywin_off - 失去焦点后，控制窗口(@key_win)标题栏的颜色
 */
void keywin_off(struct SHEET* key_win) {
	change_wtitle8(key_win, 0);
	if (key_win->flags & SHEET_CURSOR)
		fifo32_put(&key_win->task->fifo, 3);	/* 命令行窗口光标 OFF */
}

/**
 * keywin_off - 获取焦点后，控制窗口(@key_win)标题栏的颜色
 */
void keywin_on(struct SHEET* key_win) {
	change_wtitle8(key_win, 1);
	if (key_win->flags & SHEET_CURSOR)
		fifo32_put(&key_win->task->fifo, 2);	/* 命令行窗口光标 ON */
}
