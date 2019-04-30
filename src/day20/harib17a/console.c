#include <stdio.h>
#include <string.h>
#include "console.h"
#include "timer.h"
#include "mtask.h"
#include "memory.h"
#include "fifo.h"
#include "file.h"
#include "window.h"
#include "naskfunc.h"
#include "graphic.h"
#include "asmhead.h"
#include "dsctbl.h"

void console_task(struct SHEET* sheet, unsigned int memtotal) {
	struct TIMER* timer;
	struct TASK* task = task_now();
	int i, fifobuf[128];
	char cmdline[30];
	int* fat = (int*)memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	cons.sht = sheet;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
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
					timer_init(timer, &task->fifo, 0);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_FFFFFF;
				} else {
					timer_init(timer, &task->fifo, 1);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_000000;
				}
				timer_settime(timer, 50);
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
					if (cons.cur_x < 240) {
						cmdline[(cons.cur_x - 16) >> 3] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
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
	} else if (strcmp(cmdline, "hlt") == 0) {	/* 启动应用程序 hlt.hrb */
		cmd_hlt(cons, fat);
	} else if (cmdline[0] != 0) {
		/* 不是命令，也不是空行 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Bad command.", 30);
		cons_newline(cons);
		cons_newline(cons);
	}
}

void cmd_mem(struct CONSOLE* cons, unsigned int memtotal) {
	char s[30];
	sprintf(s, "total %dMB", memtotal >> 20);
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
	cons_newline(cons);
	sprintf(s, "free %dKB", memman_total(memman) >> 10);
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
	cons_newline(cons);
	cons_newline(cons);
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
				sprintf(s, "%s.%s %7d", name, finfo[x].ext, finfo[x].size);
				putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
				cons_newline(cons);
			}
		}
	}
	cons_newline(cons);
}

void cmd_type(struct CONSOLE* cons, int* fat, char* cmdline) {
	struct FILEINFO* finfo = file_search(cmdline + 5, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
	char* p;
	int y;
	if (finfo) {	/* 找到文件 */
		p = (char*)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
		for (y = 0; y < finfo->size; y++) {	/* 逐字输出 */
			cons_putchar(cons, p[y], 1);
		}
		memman_free_4k(memman, (int)p, finfo->size);
	} else {	/* 没有找到文件 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
	}
	cons_newline(cons);
	cons_newline(cons);
}

void cmd_hlt(struct CONSOLE* cons, int* fat) {
	struct FILEINFO* finfo = file_search("hlt.hrb", (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
	char* p;
	if (finfo) {	/* 找到文件 */
		p = (char*)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
		set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
		farjmp(0, 1003 << 3);
		memman_free_4k(memman, (int)p, finfo->size);
	} else {	/* 没有找到文件 */
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
		cons_newline(cons);
	}
	cons_newline(cons);
}
