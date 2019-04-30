#ifndef __FIFO_H
#define __FIFO_H

/* 不能添加 mtask.h 的原因： 会报" field `fifo' has incomplete type"错误，因为 FIFO32 和 TASK 互相引用*/

struct FIFO32 {
	int* buf;
	int head, tail, size, free, flags;
	struct TASK* task;	/* 记录要唤醒的任务 */
};

#define FLAGS_OVERRUN	0x0001

void fifo32_init(struct FIFO32* fifo, int size, int* buf, struct TASK* task);
int fifo32_put(struct FIFO32* fifo, int data);
int fifo32_get(struct FIFO32* fifo);
int fifo32_status(struct FIFO32* fifo);

#endif
