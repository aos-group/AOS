#include "mtask.h"
#include "timer.h"
#include "naskfunc.h"

int mt_tr;
struct TIMER* mt_timer;

void mt_init(void) {
	mt_timer = timer_alloc();
	/* 这里没有必要使用 timer_init */
	timer_settime(mt_timer, 2);
	mt_tr = 3 << 3;
}

void mt_taskswitch(void) {
	if (mt_tr == 3 << 3) {
		mt_tr = 4 << 3;
	} else {
		mt_tr = 3 << 3;
	}
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);
}
