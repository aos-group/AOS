#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "sheet.h"
#include "timer.h"

struct CONSOLE {
	struct SHEET* sht;
	int cur_x, cur_y, cur_c;
	struct TIMER* timer;
};

void console_task(struct SHEET* sheet);
void cons_putchar(int chr, char move);
void cons_newline();
void cons_runcmd(char* cmdline);
void cmd_mem();
void cmd_cls();
void cmd_dir();
int cmd_app(char* cmdline);
void cons_putstr0(char* s);
void cons_putstr1(char* s, int l);
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
void hrb_api_linewin(struct SHEET* sht, int x0, int y0, int x1, int y1, int col);
void cmd_exit();
void cmd_start(char* cmdline);
void cmd_ncst(char* cmdline);
void cons_langmode(char* cmdline);

#endif
