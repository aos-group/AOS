/* 对键盘和鼠标的处理 */
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
		/*		1100 1000	*/
		if ((dat & 0xc8) == 0x08) {
			/**
			 * 判断第一字节对移动有反应的部分是否在 0 ~ 3 的范围内
			 * 判断第一字节对点击有反应的部分是否在 8 ~ F 的范围内 
			 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
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

		/* 对数据进行处理 */
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0)
			mdec->x |= 0xffffff00;
		if ((mdec->buf[0] & 0x20) != 0)
			mdec->y |= 0xffffff00;
		mdec->y = -mdec->y;			/* 鼠标的 y 方向与画面符号相反 */
		return 1;
	}
	return -1;
}
