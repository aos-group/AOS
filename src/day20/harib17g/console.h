#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "sheet.h"

struct CONSOLE {
	struct SHEET* sht;
	int cur_x, cur_y, cur_c;
};

void console_task(struct SHEET* sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE* cons, int chr, char move);
void cons_newline(struct CONSOLE* cons);
void cons_runcmd(char* cmdline, struct CONSOLE* cons, int* fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE* cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE* cons);
void cmd_dir(struct CONSOLE* cons);
void cmd_type(struct CONSOLE* cons, int* fat, char* cmdline);
int cmd_app(struct CONSOLE* cons, int* fat, char* cmdline);

#endif
