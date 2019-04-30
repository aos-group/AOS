#include "timer.h"
#include "naskfunc.h"

struct TIMERCTL timerctl;

void init_pit(void) {
	int i;
	io_out8(PIT_CTRL, 0x34);
	/* 100Hz */
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next = 0xffffffff;
	timerctl.using = 0;
	for (i = 0; i < MAX_TIMER; i++)
		timerctl.timers0[i].flags = TIMER_FLAGS_UNUSE;
}

struct TIMER* timer_alloc(void) {
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == TIMER_FLAGS_UNUSE) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
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
	timerctl.using++;
	t = timerctl.t0;
	s = 0;
	while (t != 0 && timer->timeout > t->timeout) {
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
