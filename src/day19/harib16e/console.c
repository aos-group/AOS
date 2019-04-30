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
	int i, fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
	char s[30], cmdline[30], name[9], *p;
	struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
	int x, y;
	struct FILEINFO* finfo = (struct FILEINFO*)(ADR_DISKIMG + 0x002600);
	int* fat = (int*)memman_alloc_4k(memman, 4 * 2880);
	struct SEGMENT_DESCRIPTOR* gdt = (struct SEGMENT_DESCRIPTOR*)ADR_GDT;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char*)(ADR_DISKIMG + 0x000200));

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
							p = (char*)memman_alloc_4k(memman, finfo[x].size);
							file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
							cursor_x = 8;
							for (y = 0; y < finfo[x].size; y++) {	/* 逐字输出 */
								s[0] = p[y];
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
							memman_free_4k(memman, (int)p, finfo[x].size);
						} else {	/* 没有找到文件 */
							putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
							cursor_y = cons_newline(cursor_y, sheet);
						}
						cursor_y = cons_newline(cursor_y, sheet);
					} else if (strcmp(cmdline, "hlt") == 0) {	/* 启动应用程序 hlt.hrb */
						strcpy(s, "HLT     HRB ");
						for (x = 0; x < 224; x++) {
							if (finfo[x].name[0] == 0x00 || ((finfo[x].type & 0x18) == 0 && strcmp(finfo[x].name, s) == 0)) {
								break;
							}
						}
						if (x < 224 && finfo[x].name[0] != 0x00) {	/* 找到文件 */
							p = (char*)memman_alloc_4k(memman, finfo[x].size);
							file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char*)(ADR_DISKIMG + 0x003e00));
							set_segmdesc(gdt + 1003, finfo[x].size - 1, (int)p, AR_CODE32_ER);
							farjmp(0, 1003 << 3);
							memman_free_4k(memman, (int)p, finfo[x].size);
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
