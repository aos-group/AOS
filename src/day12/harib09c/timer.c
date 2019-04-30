#include "timer.h"
#include "naskfunc.h"

struct TIMERCTL timerctl;

void init_pit(void) {
	io_out8(PIT_CTRL, 0x34);
	/* 100Hz */
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.timeout = 0;
}

void settimer(unsigned int timeout, struct FIFO8* fifo, unsigned char data) {
	int eflags;
	eflags = io_load_eflags();
	io_cli();
	timerctl.timeout = timeout;
	timerctl.fifo = fifo;
	timerctl.data = data;
	io_store_eflags(eflags);
}
