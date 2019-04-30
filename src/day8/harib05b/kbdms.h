#ifndef __KBDMS_H
#define __KBDMS_H

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYSTA_WRITE_MODE		0x60 		/* 模式设定的指令 */
#define KBC_MODE				0x47		/* 鼠标模式的模式号码 */
#define	KEYCMD_SENDTO_MOUSE		0xd4
#define	MOUSECMD_ENABLE			0xf4

struct MOUSE_DEC {
	unsigned char buf[3], phase;
};

void wait_KBC_sendready(void);
void init_keyboard(void);
void enable_mouse(struct MOUSE_DEC* mdec);
int mouse_decode(struct MOUSE_DEC* mdec, unsigned char dat);

#endif
