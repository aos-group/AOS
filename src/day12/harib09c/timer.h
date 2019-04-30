#ifndef __TIMER_H
#define __TIMER_H

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL {
	unsigned int count;
	unsigned int timeout;
	struct FIFO8* fifo;
	unsigned char data;
};

void init_pit(void);
void settimer(unsigned int timeout, struct FIFO8* fifo, unsigned char data);

#endif
