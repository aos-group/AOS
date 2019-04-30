#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "sheet.h"

struct CONSOLE {
	struct SHEET* sht;
	int cur_x, cur_y, cur_c;
};

extern struct CONSOLE* console;

void console_task(struct SHEET* sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE* cons, int chr, char move);
void cons_newline(struct CONSOLE* cons);
void cons_runcmd(char* cmdline, struct CONSOLE* cons, int* fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE* cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE* cons);
void cmd_dir(struct CONSOLE* cons);
void cmd_type(struct CONSOLE* cons, int* fat, char* cmdline);
int cmd_app(struct CONSOLE* cons, int* fat, char* cmdline);
void cons_putstr0(struct CONSOLE* cons, char* s);
void cons_putstr1(struct CONSOLE* cons, char* s, int l);
int* hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
void hrb_api_linewin(struct SHEET* sht, int x0, int y0, int x1, int y1, int col);

#endif
