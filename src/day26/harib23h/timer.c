#include "timer.h"
#include "naskfunc.h"

struct TIMERCTL timerctl;

void init_pit(void) {
	int i;
	struct TIMER* t;
	io_out8(PIT_CTRL, 0x34);
	/* 100Hz 	1 / 100 * 1.93180 * 10^6 = 11932 = 0x2e9c */
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++)
		timerctl.timers0[i].flags = TIMER_FLAGS_UNUSE;
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
}

struct TIMER* timer_alloc(void) {
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == TIMER_FLAGS_UNUSE) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			timerctl.timers0[i].flags2 = 0;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER* timer) {
	timer->flags = TIMER_FLAGS_UNUSE;
}

void timer_init(struct TIMER* timer, struct FIFO32* fifo, int data) {
	timer->fifo = fifo;
	timer->data = data;
}

void timer_settime(struct TIMER* timer, unsigned int timeout) {
	int e;
	struct TIMER* t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	t = timerctl.t0;
	s = 0;
	while (timer->timeout > t->timeout) {		/* 不需要每次判断 t 了 */
		s = t;
		t = t->next;
	}
	if (s == 0) {
		timerctl.t0 = timer;
		timerctl.next = timer->timeout;
	} else {
		s->next = timer;
	}
	timer->next = t;
	io_store_eflags(e);
}

/**
 * @return: 1: 取消成功
 *			0: 不需要取消处理
 */	
int timer_cancel(struct TIMER* timer) {
	int e, ret = 0;
	struct TIMER* t;
	e = io_load_eflags();
	io_cli();	/* 在设置过程中禁止改变定时器状态 */
	if (timer->flags == TIMER_FLAGS_USING) {	/* 是否需要取消? */
		if (timer == timerctl.t0) {
			/* 第一个定时器的取消处理 */
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		} else {
			/* 非第一个定时器的取消处理 */
			/* 找到 timer 前一个定时器 */
			t = timerctl.t0;
			while (1) {
				if (t->next == timer)
					break;
				t = t->next;
			}
			t->next = timer->next;
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		ret = 1;
	}
	io_store_eflags(e);
	return ret;
}

/**
 * timer_cancelall - 取消注册到 @fifo 上所有可取消的定时器
 */
void timer_cancelall(struct FIFO32* fifo) {
	int e, i;
	struct TIMER* t;
	e = io_load_eflags();
	io_cli();	/* 在设置过程中禁止改变定时器状态 */
	for (i = 0; i < MAX_TIMER; i++) {
		t = &timerctl.timers0[i];
		if (t->flags != TIMER_FLAGS_UNUSE && t->flags2 && t->fifo == fifo) {
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
}
