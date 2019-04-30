#ifndef __TIMER_H
#define __TIMER_H

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define MAX_TIMER	500

#define TIMER_FLAGS_UNUSE	0
#define TIMER_FLAGS_ALLOC	1	/* 已配置状态 */
#define TIMER_FLAGS_USING	2	/* 定时器运行中 */

struct TIMER {
	unsigned int timeout, flags;
	struct FIFO32* fifo;
	int data;
};

/**
 * TIMERCTL - 计时器控制器
 * @count: 时钟中断计数
 * @next: 离当前时间最近的定时中断计数
 * @using: 记录现在的定时器有几个处于活动中
 * @timers0: 存放定时器信息
 * @timers: 将定时器按照时间排序
 */
struct TIMERCTL {
	unsigned int count, next, using;
	struct TIMER* timers[MAX_TIMER];
	struct TIMER timers0[MAX_TIMER];
};

void init_pit(void);
struct TIMER* timer_alloc(void);
void timer_free(struct TIMER* timer);
void timer_init(struct TIMER* timer, struct FIFO32* fifo, int data);
void timer_settime(struct TIMER* timer, unsigned int timeout);

#endif
