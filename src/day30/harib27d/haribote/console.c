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

extern struct FIFO32 fifo_a;
extern struct SHTCTL* shtctl;
extern struct TASKCTL* taskctl;

void console_task(struct SHEET* sheet) {
	struct TASK* task = task_now();
	int i;
	char cmdline[30];
	struct FILEHANDLE fhandle[8];
	extern unsigned char* nihongo, *chinese;
	struct CONSOLE cons;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	cons.sht = sheet;
	task->cons = &cons;
	task->cmdline = cmdline;

	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;		/* 未使用标记 */
	}
	task->fhandle = fhandle;

	if (chinese[4096] != 0xff) {	/* 是否载入了中文字库？ */
		task->langmode = 3;
	} else if (nihongo[4096] != 0xff) {	/* 是否载入了日文字库？ */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}

	/* 显示提示符 */
	cons_putchar('>', 1);

	while (1) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) {	/* 光标用定时器 */
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
				if (cons.sht != 0)
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			} else if (i == 4) {	/* 点击命令行窗口的 "x" 按钮 */
				cmd_exit(&cons);
			} else if (256 <= i && i < 512) {	/* 键盘数据(通过任务 A) */
				if (i == 8 + 256) {	/* 退格键 */
					if (cons.cur_x > 16) {
						cons_putchar(' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {	/* 回车键 */
					cons_putchar(' ', 0);
					cmdline[(cons.cur_x - 16) >> 3] = 0;
					cons_newline();
					cons_runcmd(cmdline);
					if (cons.sht == 0)		/* 当命令执行完毕后，如果没有命令行窗口，立即结束命令行窗口任务 */
						cmd_exit(&cons);
					cons_putchar('>', 1);
				} else {	/* 一般字符 */
					cmdline[(cons.cur_x - 16) >> 3] = i - 256;
					cons_putchar(i - 256, 1);
				}
			}
			/* 光标再显示 */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0)
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

/**
 * cons_putchar - 往命令行输出字符
 * @cons: 
 * @chr:
 * @move: 光标是否后移
 */
void cons_putchar(int chr, char move) {
	char s[2];
	struct CONSOLE* cons = task_now()->cons;
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {		/* 制表符 */
		do {
			if (cons->sht != 0){
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline();
			}
		} while (((cons->cur_x - 8) & 0x1f) != 0);	/* 不被 32 整除 */
	} else if (s[0] == 0x0a) {	/* 换行 */
		cons_newline();
	} else if (s[0] == 0x0d) {	/* 回车 */
		/* 这里暂且不进行任何操作 */
	} else {	/* 一般字符 */
		if (cons->sht != 0){
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if (move != 0) {
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline();
			}
		}
	}
}

void cons_newline() {
	int x, y;
	struct TASK* task = task_now();
	struct CONSOLE* cons = task->cons;
	struct SHEET* sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
	} else {	/* 滚动 */
		if (sheet != 0) {
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
	}
	cons->cur_x = 8;
	if (task->langmode != 0 && task->langbyte1 != 0) {
		/* 当字符串很长时，可能在全角字符的第 1 个字节处就遇到自动换行了，这样一来当收到第 2 个字节时，字模的左半部分就会画到命令行窗口外面去 */
		cons->cur_x += 8;
	}
}

void cons_runcmd(char* cmdline) {
	struct CONSOLE* cons = task_now()->cons;
	if (strcmp(cmdline, "mem") == 0 && cons->sht != 0) {	/* mem 命令 */
		cmd_mem();
	} else if (strcmp(cmdline, "cls") == 0 && cons->sht != 0) {	/* cls 命令 */
		cmd_cls();
	} else if (strcmp(cmdline, "dir") == 0 && cons->sht != 0) {	/* dir 命令 */
		cmd_dir();
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit();
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cmdline);
	} else if (strncmp(cmdline, "ncst ", 5) == 0) {		/* 不打开新命令行的 start 命令，no console start */
		cmd_ncst(cmdline);
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cons_langmode(cmdline);
	} else if (cmdline[0] != 0) {	/* 不是空行 */
		if (cmd_app(cmdline) == 0) {
			/* 不是命令，不是应用程序，也不是空行 */
			cons_putstr0("Bad command.\n\n");
		}
	}
}

void cons_langmode(char* cmdline) {
	struct TASK* task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 3) {
		task->langmode = mode;
	} else {
		cons_putstr0("mode number error.\n");
	}
	cons_newline();
}

void cmd_mem() {
	char s[60];
	sprintf(s, "total %dMB\nfree %dKB\n\n", memtotal >> 20, memman_total(memman) >> 10);
	cons_putstr0(s);
}

void cmd_cls() {
	int x, y;
	struct CONSOLE* cons = task_now()->cons;
	struct SHEET* sheet = cons->sht;
	for (y = 28; y < 28 + 112; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
}

void cmd_dir() {
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
				cons_putstr0(s);
			}
		}
	}
	cons_newline();
}

int cmd_app(char* cmdline) {
	struct FILEINFO* finfo;
	char* p, name[18], *q;
	int i, segsiz, datsiz, esp, dathrb, appsiz;
	struct TASK* task = task_now();
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
		appsiz = finfo->size;
		p = file_loadfile2(finfo->clustno, &appsiz);
		if (appsiz >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
			segsiz = *((int*)(p + 0x0000));
			esp = *((int*)(p + 0x000c));
			datsiz = *((int*)(p + 0x0010));
			dathrb = *((int*)(p + 0x0014));
			q = (char*)memman_alloc_4k(memman, segsiz);
			task->ds_base = (int)q;
			set_segmdesc(task->ldt + 0, appsiz, (int)p, AR_CODE32_ER_USER);
			set_segmdesc(task->ldt + 1, segsiz - 1, (int)q, AR_DATA32_RW_USER);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			/* 当应用程序结束时，查询所有的图层，如果图层的 task 为将要结束的应用程序任务，则关闭该图层 */
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & (SHEET_USE | SHEET_FROM_APPLICATION)) == (SHEET_USE | SHEET_FROM_APPLICATION) && sht->task == task) {
					sheet_free(sht);
				}
			}
			/* 释放文件资源 */
			for (i = 0; i < 8; i++) {
				if (task->fhandle[i].buf) {
					memman_free_4k(memman, (int)task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int)q, segsiz);
			task->langbyte1 = 0;	/* 当程序出现 bug 或者强制结束时可能出现在显示全角字符第 1 个字节时停止的情况 */
		} else {
			cons_putstr0(".hrb file format error.\n");
		}
		memman_free_4k(memman, (int)p, appsiz);
		cons_newline();
		return 1;
	}
	return 0;
}

void cmd_exit() {
	struct TASK* task = task_now();
	struct CONSOLE* cons = task->cons;
	if (cons->sht != 0)
		timer_cancel(cons->timer);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(&fifo_a, cons->sht - shtctl->sheets0 + 768);	/* 768 - 1023 */
	} else {
		fifo32_put(&fifo_a, task - taskctl->tasks0 + 1024);	/* 1024 - 2023 */
	}
	io_sti();
	while (1) {
		task_sleep(task);
	}
}

void cmd_start(char* cmdline) {
	struct SHEET* sht = open_console(shtctl);
	struct FIFO32* fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* 将命令行输入的字符逐字复制到新的命令行窗口中 */
	for (i = 6; cmdline[i]; i++)
		fifo32_put(fifo, cmdline[i] + 256);
	fifo32_put(fifo, 10 + 256);		/* 回车 */
	cons_newline();
}

void cmd_ncst(char* cmdline) {
	struct TASK* task = open_constask(0);
	struct FIFO32* fifo = &task->fifo;
	int i;
	/* 将命令行输入的字符逐字复制到新的命令行窗口中 */
	for (i = 5; cmdline[i]; i++)
		fifo32_put(fifo, cmdline[i] + 256);
	fifo32_put(fifo, 10 + 256);		/* 回车 */
	cons_newline();
}

void cons_putstr0(char* s) {
	for (; *s; s++) {
		cons_putchar(*s, 1);
	}
}

void cons_putstr1(char* s, int l) {
	int i;
	for (i = 0; i < l; i++, s++) {
		cons_putchar(*s, 1);
	}
}

/**
 * hrb_api - 应用程序 API
 */
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
	int i;
	struct TASK* task = task_now();
	int ds_base = task->ds_base;
	struct SHEET* sht;
	struct CONSOLE* cons = task->cons;
	struct FILEINFO* finfo;
	struct FILEHANDLE* fh;
	int* reg = &eax + 1;	/* eax 后面的地址 */
	/* 强行改写通过 pushad 保存的值 */
	/* reg[0]: edi, reg[1]: esi, reg[2]: ebp, reg[3]: esp */
	/* reg[4]: ebx, reg[5]: edx, reg[6]: ecx, reg[7]: eax */
	if (edx == 1) {
		/**
		 * edx == 1 - 显示单个字符
		 * @al: 字符编码
		 */
		cons_putchar(eax & 0xff, 1);
	} else if (edx == 2) {
		/**
		 * edx == 2 - 显示字符串 0
		 * @ebx: 字符串地址
		 */
		cons_putstr0((char*)ebx + ds_base);
	} else if (edx == 3) {
		/**
		 * edx == 3 - 显示字符串 1
		 * @ebx: 字符串地址
		 * @ecx: 字符串长度
		 */
		cons_putstr1((char*)ebx + ds_base, ecx);
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
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top);		/* 将窗口图层高度指定为当前鼠标所在的图层的高度，鼠标移到上层 */
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
		if ((ebx & 1) == 0){
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
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
					break;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) {	/* 光标用定时器 */
				/* 应用程序运行时不需要显示光标，因此总是将下次显示用的值置为 1 */
				timer_init(cons->timer, &task->fifo, 1);
				timer_settime(cons->timer, 50);
			} else if (i == 2) {	/* 光标 ON */
				cons->cur_c = COL8_FFFFFF;
			} else if (i == 3) {	/* 光标 OFF */
				cons->cur_c = -1;
			} else if (i == 4) {	/* 只关闭命令行窗口 */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(&fifo_a, cons->sht - shtctl->sheets0 + 2024);	/* 2024 - 2279 */
				cons->sht = 0;
				io_sti();
			} else if (i >= 256) {	/* 键盘数据(通过任务 A)或其他数据 */
				reg[7] = i - 256;
				break;
			}
		}
		return 0;
	} else if (edx == 16) {
		/**
		 * edx == 16 - 获取定时器
		 * @return(eax): 定时器句柄
		 */
		reg[7] = (int)timer_alloc();
		((struct TIMER*)reg[7])->flags2 = 1;	/* 允许定时器自动取消 */
	} else if (edx == 17) {
		/**
		 * edx == 17 - 设置定时器的发送数据
		 * @ebx: 定时器句柄
		 * @eax: 数据
		 */
		timer_init((struct TIMER*)ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		/**
		 * edx == 17 - 定时器时间设定
		 * @ebx: 定时器句柄
		 * @eax: 时间
		 */
		timer_settime((struct TIMER*)ebx, eax);
	} else if (edx == 19) {
		/**
		 * edx == 19 - 释放定时器
		 * @ebx: 定时器句柄
		 */
		timer_free((struct TIMER*)ebx);
	} else if (edx == 20) {
		/**
		 * edx == 20 - 蜂鸣器发声
		 * @eax: 声音频率(单位是 mHz, 即毫赫兹)
		 * 		 例如当 eax = 440000 时，则发出 440Hz 的声音
		 * 		 频率为 0 则表示停止发声
		 */
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;	/* PIT 时钟频率为 1.19318MHz */
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		/**
		 * edx == 21 - 打开文件
		 * @ebx: 文件名
		 * @return(eax) : 文件句柄，为 0 时表示打开失败
		 */
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char*)ebx + ds_base, (struct FILEINFO*)(ADR_DISKIMG + 0x002600), 224);
			if (finfo) {
				reg[7] = (int)fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_loadfile2(finfo->clustno, &fh->size);
			}
		}
	} else if (edx == 22) {
		/**
		 * edx == 22 - 关闭文件
		 * @eax: 文件句柄
		 */
		fh = (struct FILEHANDLE*)eax;
		memman_free_4k(memman, (int)fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		/**
		 * edx == 23 - 文件定位
		 * @eax: 文件句柄
		 * @ecx: 定位模式
		 *		 0: 定位的起点为文件开头
		 *		 1: 定位的起点为当前的访问位置
		 * 		 2: 定位的起点为文件末尾
		 * @ebx: 定位偏移量
		 */
		fh = (struct FILEHANDLE*)eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		} else if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		/**
		 * edx == 24 - 获取文件大小
		 * @eax: 文件句柄
		 * @ecx: 文件大小获取模式
		 * 		 0: 普通文件大小
		 * 		 1: 当前读取位置从文件开头起算的偏移量
		 * 		 2: 当前读取位置从文件末尾起算的偏移量
		 * @return(eax): 文件大小
		 */	
		fh = (struct FILEHANDLE*)eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		/**
		 * edx == 25 - 文件读取
		 * @eax: 文件句柄
		 * @ebx: 缓冲区地址
		 * @ecx: 最大读取字节数
		 * @return(eax): 本次读取到的字节数
		 */
		fh = (struct FILEHANDLE*)eax;
		for (i = 0; i < ecx && fh->pos < fh->size; i++, fh->pos++) {
			*((char*)ebx + ds_base + i) = fh->buf[fh->pos];
		}
		reg[7] = i;
	} else if (edx == 26) {
		/**
		 * edx == 26 - 获取命令行
		 * @ebx: 存放命令行内容的地址
		 * @ecx: 最多可存放多少字节
		 * @return(eax): 实际存放了多少字节
		 */
		for (i = 0; i < ecx && task->cmdline[i]; i++) {
			*((char*)ebx +ds_base + i) = task->cmdline[i];
		}
		*((char*)ebx + ds_base + i) = 0;
		reg[7] = i;
	} else if (edx == 27) {
		/**
		 * edx == 27 - 获取 langmode
		 * @return(eax): langmode
		 */
		reg[7] = task->langmode;
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
