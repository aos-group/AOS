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

struct CONSOLE* console;

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
	*((int*)0x0fec) = (int)&cons;
	console = (struct CONSOLE*)*((int*)0x0fec);

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
		q = (char*)memman_alloc_4k(memman, 64 << 10);	/* 64KB */
		*((int*)0xfe8) = (int)p;
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
		set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
		set_segmdesc(gdt + 1004, (64 << 10) - 1, (int)q, AR_DATA32_RW);
		/* 
		 * [bits 32]
		 * 		call 0x1b
		 * 		retf
		 */
		if (finfo->size >= 8 && strncmp(p + 4, "Hari", 4) == 0) {	
			p[0] = 0xe8;
			p[1] = 0x16;
			p[2] = 0x00;
			p[3] = 0x00;
			p[4] = 0x00;
			p[5] = 0xcb;
		}
		start_app(0, 1003 << 3, 64 << 10, 1004 << 3);
		memman_free_4k(memman, (int)p, finfo->size);
		memman_free_4k(memman, (int)q, 64 << 10);
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

void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
	int cs_base = *((int*)0xfe8);
	if (edx == 1) {
		cons_putchar(console, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(console, (char*)ebx + cs_base);
	} else if (edx == 3) {
		cons_putstr1(console, (char*)ebx + cs_base, ecx);
	}
}
