#ifndef __MTASK_H
#define __MTASK_H

#include "memory.h"
#include "fifo.h"
#include "console.h"
#include "dsctbl.h"

#define MAX_TASKS	1000	/* 最大任务数量 */
#define TASK_GDT0	3		/* 定义从 GDT 的几号开始分配给 TSS */
#define MAX_TASKS_LV	100		/* 同一 level 下的最大任务数量 */
#define	MAX_TASKLEVELS	10

struct FIFO32;

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

/**
 * TASK - 用来描述任务
 * @sel: 用来存放 GDT 的编号
 * @flags: 任务状态
 		0: 初始化
 		1: 阻塞
 		2: 在运行队列
 * @ds_base: 应用程序数据段选择子
 * @cons_stack: 该任务的内核栈
 * @fhandle: 该任务打开的文件句柄数组
 * @cmdline: 当前命令行的命令
 * @langmode: 语言模式
 		0: 英文模式
 		1: 日文模式 (Shift-JIS)
 		2: 日文模式 (EUC)
 		3: 中文模式 (GB2312)
 * @langbyte1: 当接收到全角字符时用来存放第 1 个字节内容的变量。当接收到半角字符，或者全角字符显示完成后，该变量被置为 0
 */
struct TASK {
	int sel, flags;
	int priority, level;
	struct FIFO32 fifo;
	struct TSS32 tss;
	struct SEGMENT_DESCRIPTOR ldt[2];
	struct CONSOLE* cons;
	int ds_base, cons_stack;
	struct FILEHANDLE* fhandle;
	char* cmdline;
	unsigned char langmode;
	unsigned char langbyte1;
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

extern struct TASKCTL* taskctl;

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
