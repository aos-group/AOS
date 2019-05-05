#include "mtask.h"
#include "timer.h"
#include "naskfunc.h"

struct TASKCTL* taskctl = 0;
struct TIMER* task_timer = 0;

struct TASK* task_init(struct MEMMAN* man) {
	int i;
	struct TASK* task, *idle;
	taskctl = (struct TASKCTL*)memman_alloc_4k(man, sizeof(struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		task = &taskctl->tasks0[i];
		task->flags = 0;
		task->sel = (TASK_GDT0 + i) << 3;	/* 3 ~ 1002 */
		task->tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) << 3;	/* 1003 ~ 2002 */
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int)&task->tss, AR_TSS32);
		set_segmdesc(gdt + TASK_GDT0 + MAX_TASKS + i, 15, (int)task->ldt, AR_LDT);
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	/* 上面初始化后才可以调用该函数 */
	task = task_alloc();
	task->flags = 2;	/* 活动中标志 */
	task->priority = 2;	/* 0.02s */
	task->level = 0;
	task_add(task);
	task_switchsub();
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(man, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int)&task_idle;
	idle->tss.es = 1 << 3;
	idle->tss.cs = 2 << 3;
	idle->tss.ss = 1 << 3;
	idle->tss.ds = 1 << 3;
	idle->tss.fs = 1 << 3;
	idle->tss.gs = 1 << 3;
	task_run(idle, MAX_TASKLEVELS - 1, 1);
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
			task->tss.iomap = 0x40000000;
			task->tss.ss0 = 0;
			return task;
		}
	}
	return 0;	/* 全部正在使用 */
}

/**
 * task_run - 运行任务
 * @task: 待运行的任务
 * @level: 待运行任务的 level, 若为负数，则不改变 level
 * @priority: 任务优先级，表示切换到该任务时可以运行多长时间，值为 0 表示不改变当前已经设定的优先级，主要是为了在唤醒休眠任务的时候使用
 */
void task_run(struct TASK* task, int level, int priority) {
	if (level < 0) {
		level = task->level;
	}
	if (priority > 0) {
		task->priority = priority;
	}
	if (task->flags == 2 && task->level != level) {	/* 改变活动中的 level */
		task_remove(task);	/* 这里执行之后 flags 的值会变为 1, 于是下面的 if 语句块也会执行 */
	}
	if (task->flags != 2) {
		task->level = level;
		task_add(task);
	}
	taskctl->lv_change = 1;		/* 下次任务切换时检查 level */
}

void task_switch(void) {
	struct TASKLEVEL* tl = &taskctl->level[taskctl->now_lv];
	struct TASK* new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
}

void task_sleep(struct TASK* task) {
	struct TASK* now_task;
	if (task->flags == 2) {		/* 如果指定任务处于唤醒状态 */
		now_task = task_now();
		task_remove(now_task);
		if (task == now_task) {
			/* 如果是让自己休眠，则需要进行任务切换 */
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}
}

struct TASK* task_now(void) {
	struct TASKLEVEL* tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

/**
 * task_add - 将任务添加到运行队列
 */
void task_add(struct TASK* task) {
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	if (tl->running < MAX_TASKS_LV) {
		tl->tasks[tl->running] = task;
		tl->running++;
		task->flags = 2;	/* 活动中 */
	}
}

/**
 * task_remove - 将任务从运行队列移除
 */
void task_remove(struct TASK* task) {
	int i;
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			break;
		}
	}
	tl->running--;
	if (i < tl->now) {
		tl->now--;	/* 需要移动成员，要相应地处理 */
	}
	if (tl->now >= tl->running) {
		tl->now = 0;	/* 如果 now 的值出现异常，则进行修正 */
	}
	task->flags = 1;	/* 休眠中 */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
}

/**
 * task_switchsub - 在任务切换时决定接下来切换到哪个 level
 */
void task_switchsub(void) {
	int i;
	/* 寻找最上层的 level */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break;
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
}

void task_idle(void) {
	while (1) {
		io_hlt();
	}
}
