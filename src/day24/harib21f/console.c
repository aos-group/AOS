#include <stdio.h>
#include <string.h>
#include "console.h"
#include "mtask.h"
#include "memory.h"
#include "fifo.h"
#include "file.h"
#include "window.h"
#include "naskfunc.h"
#include "graphic.h"
#include "asmhead.h"
#include "dsctbl.h"

struct CONSOLE* console;

void console_task(struct SHEET* sheet, unsigned int memtotal) {
	struct TASK* task = task_now();
	int i, fifobuf[128];
	char cmdline[30];
	int* fat = (int*)memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	cons.sht = sheet;
	*((int*)0x0fec) = (int)&cons;
	console = (struct CONSOLE*)*((int*)0x0fec);

	fifo32_init(&task->fifo, 128, fifobuf, task);
	cons.timer = timer_alloc();
	timer_init(cons.timer, &task->fifo, 1);
	timer_settime(cons.timer, 50);
	file_readfat(fat, (unsigned char*)(ADR_DISKIMG + 0x000200));

	/* 显示提示符 */
	cons_putchar(&cons, '>', 1);

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
					timer_init(cons.timer, &task->fifo, 0);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_FFFFFF;
				} else {
					timer_init(cons.timer, &task->fifo, 1);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_000000;
				}
				timer_settime(cons.timer, 50);
			} else if (i == 2) {	/* 光标 ON */
				cons.cur_c = COL8_FFFFFF;
			} else if (i == 3) {	/* 光标 OFF */
				cons.cur_c = -1;
				boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			} else if (256 <= i && i < 512) {	/* 键盘数据(通过任务 A) */
				if (i == 8 + 256) {	/* 退格键 */
					if (cons.cur_x > 16) {
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {	/* 回车键 */
					cons_putchar(&cons, ' ', 0);
					cmdline[(cons.cur_x - 16) >> 3] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);
					cons_putchar(&cons, '>', 1);
				} else {	/* 一般字符 */
					cmdline[(cons.cur_x - 16) >> 3] = i - 256;
					cons_putchar(&cons, i - 256, 1);
				}
			}
			/* 光标再显示 */
			if (cons.cur_c >= 0)
				boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

/**
 * cons_putchar - 往命令行输出字符
 * @cons: 
 * @chr:
 * @move: 光标是否后移
 */
void cons_putchar(struct CONSOLE* cons, int chr, char move) {
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {		/* 制表符 */
		do {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		} while (((cons->cur_x - 8) & 0x1f) != 0);	/* 不被 32 整除 */
	} else if (s[0] == 0x0a) {	/* 换行 */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* 回车 */
		/* 这里暂且不进行任何操作 */
	} else {	/* 一般字符 */
		putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		if (move != 0) {
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
}

void cons_newline(struct CONSOLE* cons) {
	int x, y;
	struct SHEET* sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
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
	cons->cur_x = 8;
}

void cons_runcmd(char* cmdline, struct CONSOLE* cons, int* fat, unsigned int memtotal) {
	if (strcmp(cmdline, "mem") == 0) {	/* mem 命令 */
		cmd_mem(cons, memtotal);
	} else if (strcmp(cmdline, "cls") == 0) {	/* cls 命令 */
		cmd_cls(cons);
	} else if (strcmp(cmdline, "dir") == 0) {	/* dir 命令 */
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0) {		/* type 命令 */
		cmd_type(cons, fat, cmdline);
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* 不是命令，不是应用程序，也不是空行 */
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
}

void cmd_mem(struct CONSOLE* cons, unsigned int memtotal) {
	char s[60];
	sprintf(s, "total %dMB\nfree %dKB\n\n", memtotal >> 20, memman_total(memman) >> 10);
	cons_putstr0(cons, s);
}

void cmd_cls(struct CONSOLE* cons) {
	int x, y;
	struct SHEET* sheet = cons->sht;
	for (y = 28; y < 28 + 112; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
}

void cmd_dir(struct CONSOLE* cons) {
	struct FILEINFO* finfo = (struct FILEINFO*)(ADR_DISKIMG + 0x002600);
	int x;
	char s[30], name[9];
	for (x = 0; x < 224; x++) {
		if (finfo[x].name[0] == 0x00)	/* 不包含任何文件信息 */
			break;
		if (finfo[x].name[0] != 0xe5) {		/* 没有被删除 */
			if ((finfo[x].type & 0x18) == 0) {	/* 普通文件 */
				strncpy(name, finfo[x].name, 8);	/* 将文件名与后缀分开 */
				name[8] = 0;
				sprintf(s, "%s.%s %7d\n", name, finfo[x].ext, finfo[x].size);
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
}

void cmd_type(struct CONSOLE* cons, int* fat, char* cmdline) {
	struct FILEINFO* finfo = file_search(cmdline + 5, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
	char* p;
	if (finfo) {	/* 找到文件 */
		p = (char*)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
		cons_putstr1(cons, p, finfo->size);
		memman_free_4k(memman, (int)p, finfo->size);
	} else {	/* 没有找到文件 */
		cons_putstr0(cons, "File not found.");
	}
	cons_newline(cons);
	cons_newline(cons);
}

int cmd_app(struct CONSOLE* cons, int* fat, char* cmdline) {
	struct FILEINFO* finfo;
	char* p, name[18], *q;
	int i;
	struct TASK* task = task_now();
	int segsiz, datsiz, esp, dathrb;
	struct SHTCTL* shtctl;
	struct SHEET* sht;

	/* 根据命令行生成文件名 */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {	/* 32 */
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0;	/* 暂且将文件名的后面置为 0 */

	finfo = file_search(name, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {		/* ?? */
		/* 由于找不到文件，故在文件名后面加上 ".hrb" 后重新寻找 */
		/* 命令行中输入可以不加后缀 */
		strcpy(name + i, ".hrb");
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo) {	/* 找到文件 */
		p = (char*)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
		if (finfo->size >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
			segsiz = *((int*)(p + 0x0000));
			esp = *((int*)(p + 0x000c));
			datsiz = *((int*)(p + 0x0010));
			dathrb = *((int*)(p + 0x0014));
			q = (char*)memman_alloc_4k(memman, segsiz);
			*((int*)0xfe8) = (int)q;
			set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER_USER);
			set_segmdesc(gdt + 1004, segsiz - 1, (int)q, AR_DATA32_RW_USER);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, 1003 << 3, esp, 1004 << 3, &(task->tss.esp0));
			shtctl = (struct SHTCTL*)*((int*)0x0fe4);
			/* 当应用程序结束时，查询所有的图层，如果图层的 task 为将要结束的应用程序任务，则关闭该图层 */
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & (SHEET_USE | SHEET_FROM_APPLICATION)) == (SHEET_USE | SHEET_FROM_APPLICATION) && sht->task == task) {
					sheet_free(sht);
				}
			}
			memman_free_4k(memman, (int)q, segsiz);
		} else {
			cons_putstr0(cons, ".hrb file format error.\n");
		}
		memman_free_4k(memman, (int)p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	return 0;
}

void cons_putstr0(struct CONSOLE* cons, char* s) {
	for (; *s; s++) {
		cons_putchar(cons, *s, 1);
	}
}

void cons_putstr1(struct CONSOLE* cons, char* s, int l) {
	int i;
	for (i = 0; i < l; i++, s++) {
		cons_putchar(cons, *s, 1);
	}
}

/**
 * hrb_api - 应用程序 API
 */
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
	int ds_base = *((int*)0xfe8);
	int i;
	struct TASK* task = task_now();
	struct SHTCTL* shtctl = (struct SHTCTL*)*((int*)0x0fe4);
	struct SHEET* sht;
	int* reg = &eax + 1;	/* eax 后面的地址 */
	/* 强行改写通过 pushad 保存的值 */
	/* reg[0]: edi, reg[1]: esi, reg[2]: ebp, reg[3]: esp */
	/* reg[4]: ebx, reg[5]: edx, reg[6]: ecx, reg[7]: eax */
	if (edx == 1) {
		/**
		 * edx == 1 - 显示单个字符
		 * @al: 字符编码
		 */
		cons_putchar(console, eax & 0xff, 1);
	} else if (edx == 2) {
		/**
		 * edx == 2 - 显示字符串 0
		 * @ebx: 字符串地址
		 */
		cons_putstr0(console, (char*)ebx + ds_base);
	} else if (edx == 3) {
		/**
		 * edx == 3 - 显示字符串 1
		 * @ebx: 字符串地址
		 * @ecx: 字符串长度
		 */
		cons_putstr1(console, (char*)ebx + ds_base, ecx);
	} else if (edx == 4) {
		/**
		 * edx == 4 - 结束程序
		 */
		return &(task->tss.esp0);
	} else if (edx == 5) {
		/**
		 * edx == 5 - 显示窗口
		 * @ebx: 窗口缓冲区
		 * @esi: 窗口宽度
		 * @edi: 窗口高度
		 * @eax: 透明色
		 * @ecx: 窗口名称
		 * @return(eax: 窗口句柄
		 */
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= SHEET_FROM_APPLICATION;
		sheet_setbuf(sht, (char*)ebx + ds_base, esi, edi, eax);
		make_window8((char*)ebx + ds_base, esi, edi, (char*)ecx + ds_base, 0);
		sheet_slide(sht, 200, 200);
		sheet_updown(sht, 3);
		reg[7] = (int)sht;
	} else if (edx == 6) {
		/**
		 * edx == 6 - 在窗口上显示字符
		 * @ebx: 窗口句柄，地址为奇数不自动刷新窗口，地址为偶数自动刷新窗口
		 * @esi: 显示位置的 x 坐标
		 * @edi: 显示位置的 y 坐标
		 * @eax: 色号
		 * @ecx: 字符串长度
		 * @ebp: 字符串
		 */
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char*)ebp + ds_base);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
	} else if (edx == 7) {
		/**
		 * edx == 7 - 描绘方块
		 * @ebx: 窗口句柄，地址为奇数不自动刷新窗口，地址为偶数自动刷新窗口
		 * @eax: x0
		 * @ecx: y0
		 * @esi: x1
		 * @edi: y1
		 * @ebp: 色号
		 */
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	} else if (edx == 8) {
		/**
		 * edx == 8 - memman 的初始化
		 * @ebx: memman 的地址
		 * @eax: memman 所管理内存空间的起始地址
		 * @ecx: memman 所管理内存空间的字节数
		 */
		memman_init((struct MEMMAN*)(ebx + ds_base));
		ecx &= 0xfffffff0;	/* 以 16 字节为单位 */
		memman_free((struct MEMMAN*)(ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		/**
		 * edx == 9 - malloc
		 * @ebx: memman 的地址
		 * @ecx: 需要请求的字节数
		 * @return(eax): 分配到的内存空间地址
		 */
		ecx = (ecx + 0x0f) & 0xfffffff0;	/* 以 16 字节为单位进位取整 */
		reg[7] = memman_alloc((struct MEMMAN*)(ebx + ds_base), ecx);
	} else if (edx == 10) {
		/**
		 * edx == 10 - free
		 * @ebx: memman 的地址
		 * @eax: 需要释放的内存空间地址
		 * @ecx: 需要释放的字节数
		 */
		ecx = (ecx + 0x0f) & 0xfffffff0;	/* 以 16 字节为单位进位取整 */
		memman_free((struct MEMMAN*)(ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		/**
		 * edx == 11 - 在窗口中画点
		 * @ebx: 窗口句柄，地址为奇数不自动刷新窗口，地址为偶数自动刷新窗口
		 * @esi: 显示位置的 x 坐标
		 * @edi: 显示位置的 y 坐标
		 * @eax: 色号
		 */
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0)
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
	} else if (edx == 12) {
		/**
		 * edx == 12 - 刷新窗口
		 * @ebx: 窗口句柄
		 * @eax: x0
		 * @ecx: y0
		 * @esi: x1
		 * @edi: y1
		 */
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		/**
		 * edx == 13 - 在窗口上画直线
		 * @ebx: 窗口句柄，地址为奇数不自动刷新窗口，地址为偶数自动刷新窗口
		 * @eax: x0
		 * @ecx: y0
		 * @esi: x1
		 * @edi: y1
		 * @ebp: 色号
		 */
		sht = (struct SHEET*)(ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	} else if (edx == 14) {
		/**
		 * edx == 14 - 关闭窗口
		 * @ebx: 窗口句柄，地址为奇数不自动刷新窗口，地址为偶数自动刷新窗口
		 */
		sheet_free((struct SHEET*)ebx);
	} else if (edx == 15) {
		/**
		 * edx == 15 - 键盘输入
		 * @eax: 0 -- 没有键盘输入时返回 -1, 不休眠
		 *		 1 -- 休眠直到发生键盘输入
		 * @return(eax): 输入的字符编码
		 */
		while (1) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax) {
					task_sleep(task);
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) {	/* 光标用定时器 */
				/* 应用程序运行时不需要显示光标，因此总是将下次显示用的值置为 1 */
				timer_init(console->timer, &task->fifo, 1);
				timer_settime(console->timer, 50);
			} else if (i == 2) {	/* 光标 ON */
				console->cur_c = COL8_FFFFFF;
			} else if (i == 3) {	/* 光标 OFF */
				console->cur_c = -1;
			} else if (256 <= i && i < 512) {	/* 键盘数据(通过任务 A) */
				reg[7] = i - 256;
				return 0;
			}
		}
	}
	return 0;
}

void hrb_api_linewin(struct SHEET* sht, int x0, int y0, int x1, int y1, int col) {
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;
	if (dx >= dy) {
		len = dx + 1;
		dx = x0 > x1 ? -1024 : 1024;
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		dy = y0 > y1 ? -1024 : 1024;
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}
}
