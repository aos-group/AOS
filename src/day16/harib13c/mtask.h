#ifndef __MTASK_H
#define __MTASK_H
#include "memory.h"

#define MAX_TASKS	1000	/* 最大任务数量 */
#define TASK_GDT0	3		/* 定义从 GDT 的几号开始分配给 TSS */

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK {
	int sel, flags;		/* sel 用来存放 GDT 的编号 */
	struct TSS32 tss;
};

struct TASKCTL {
	int running;	/* 正在运行的任务数量 */
	int now;	/* 当前正在运行的是哪个任务 */
	struct TASK* tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};

struct TASK* task_init(struct MEMMAN* memman);
struct TASK* task_alloc(void);
void task_run(struct TASK* task);
void task_switch(void);
void task_sleep(struct TASK* task);

#endif
