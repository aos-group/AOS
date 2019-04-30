#include "keyboard.h"
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

void enable_mouse(void) {
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);		/* 顺利的话，键盘控制器会返回 ACK(0xfa) */
}
