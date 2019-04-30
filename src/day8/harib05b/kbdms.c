#include "kbdms.h"
#include "naskfunc.h"

/**
 * wait_KBC_sendready - 等待键盘控制电路准备完毕
 */
void wait_KBC_sendready(void) {
	while (1) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
			break;
	}
}

void init_keyboard(void) {
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYSTA_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
}

void enable_mouse(struct MOUSE_DEC* mdec) {
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);		/* 顺利的话，键盘控制器会返回 ACK(0xfa) */
	mdec->phase = 0;	/* 等待 0xfa 的阶段 */
}

int mouse_decode(struct MOUSE_DEC* mdec, unsigned char dat) {
	if (mdec->phase == 0) {
		/* 等待鼠标的 0xfa 的状态 */
		if (dat == 0xfa)
			mdec->phase = 1;
		return 0;
	} else if (mdec->phase == 1) {
		/* 等待鼠标的第一字节 */
		mdec->buf[0] = dat;
		mdec->phase = 2;
		return 0;
	} else if (mdec->phase == 2) {
		/* 等待鼠标的第二字节 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	} else if (mdec->phase == 3) {
		/* 等待鼠标的第三字节 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		return 1;
	}
	return -1;
}
