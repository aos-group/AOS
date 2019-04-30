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
	for (i = 0; i < MAX_TIMER; i++)
		timerctl.timer[i].flags = TIMER_FLAGS_UNUSE;
}

struct TIMER* timer_alloc(void) {
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timer[i].flags == TIMER_FLAGS_UNUSE) {
			timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timer[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER* timer) {
	timer->flags = TIMER_FLAGS_UNUSE;
}

void timer_init(struct TIMER* timer, struct FIFO8* fifo, unsigned char data) {
	timer->fifo = fifo;
	timer->data = data;
}

void timer_settime(struct TIMER* timer, unsigned int timeout) {
	timer->timeout = timeout;
	timer->flags = TIMER_FLAGS_USING;
}
