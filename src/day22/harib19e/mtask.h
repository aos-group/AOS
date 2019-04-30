#ifndef __MTASK_H
#define __MTASK_H

#include "memory.h"
#include "fifo.h"

#define MAX_TASKS	1000	/* 最大任务数量 */
#define TASK_GDT0	3		/* 定义从 GDT 的几号开始分配给 TSS */
#define MAX_TASKS_LV	100
#define	MAX_TASKLEVELS	10

struct FIFO32;

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

/**
 * TASK - 
 * @sel: 用来存放 GDT 的编号
 * @flags: 任务状态
 		0: 初始化
 		1: 阻塞
 		2: 在运行队列
 * @priority:
 * @level:
 * @fifo:
 * @tss:
 */
struct TASK {
	int sel, flags;		/* sel 用来存放 GDT 的编号 */
	int priority, level;
	struct FIFO32 fifo;
	struct TSS32 tss;
};

struct TASKLEVEL {
	int running;	/* 正在运行的任务数量 */
	int now;	/* 当前正在运行的是哪个任务 */
	struct TASK* tasks[MAX_TASKS_LV];
};

struct TASKCTL {
	int now_lv;		/* 现在活动的 level */
	char lv_change;	/* 在下次任务切换时是否需要改变 level */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

struct TASK* task_init(struct MEMMAN* man);
struct TASK* task_alloc(void);
void task_run(struct TASK* task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK* task);
struct TASK* task_now(void);
void task_add(struct TASK* task);
void task_remove(struct TASK* task);
void task_switchsub(void);
void task_idle(void);

#endif
