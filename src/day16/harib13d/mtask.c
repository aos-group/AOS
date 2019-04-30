#include "mtask.h"
#include "timer.h"
#include "dsctbl.h"
#include "naskfunc.h"

struct TASKCTL* taskctl;
struct TIMER* task_timer;

struct TASK* task_init(struct MEMMAN* memman) {
	int i;
	struct TASK* task;
	struct SEGMENT_DESCRIPTOR* gdt = (struct SEGMENT_DESCRIPTOR*)ADR_GDT;
	taskctl = (struct TASKCTL*)memman_alloc_4k(memman, sizeof(struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) << 3;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);
	}
	/* 上面初始化后才可以调用该函数 */
	task = task_alloc();
	task->flags = 2;	/* 活动中标志 */
	task->priority = 2;	/* 0.02s */
	taskctl->running = 1;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, 2);
	return task;
}

struct TASK* task_alloc(void) {
	int i;
	struct TASK* task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1;	/* 正在使用的标志 */
			task->tss.eflags = 0x00000202;	/* IF = 1 */
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0;	/* 全部正在使用 */
}

/**
 * task_run - 运行任务
 * @task: 待运行的任务
 * @priority: 任务优先级，值为 0 表示不改变当前已经设定的优先级，主要是为了在唤醒休眠任务的时候使用
 */
void task_run(struct TASK* task, int priority) {
	if (priority > 0) {
		task->priority = priority;
	}
	if (task->flags != 2) {
		task->flags = 2;
		taskctl->tasks[taskctl->running] = task;
		taskctl->running++;
	}
}

void task_switch(void) {
	struct TASK* task;
	taskctl->now++;
	if (taskctl->now == taskctl->running) {
		taskctl->now = 0;
	}
	task = taskctl->tasks[taskctl->now];
	timer_settime(task_timer, task->priority);
	if (taskctl->running >= 2) {
		farjmp(0, task->sel);
	}
}

void task_sleep(struct TASK* task) {
	int i;
	char ts = 0;
	if (task->flags == 2) {		/* 如果指定任务处于唤醒状态 */
		if (task == taskctl->tasks[taskctl->now]) {
			ts = 1;		/* 让自己休眠的话，需要进行任务切换 */
		}
		for (i = 0; i < taskctl->running; i++) {
			if (taskctl->tasks[i] == task) {
				break;
			}
		}
		taskctl->running--;
		if (i < taskctl->now) {
			taskctl->now--;		/* 需要移动成员，要相应地处理 */
		}
		/* 移动成员 */
		for (; i < taskctl->running; i++) {
			taskctl->tasks[i] = taskctl->tasks[i + 1];
		}
		task->flags = 1;
		if (ts) {
			if (taskctl->now >= taskctl->running) {
				taskctl->now = 0;
			}
			farjmp(0, taskctl->tasks[taskctl->now]->sel);
		}
	}
}
