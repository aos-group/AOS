#include "int.h"

void init_pic(void) {
	io_out8(PIC0_IMR, 0xff);	/* 禁止所有中断 */
	io_out8(PIC1_IMR, 0xff);	/* 禁止所有中断 */

	io_out8(PIC0_ICW1, 0x11);	/* 边沿触发模式 */
	io_out8(PIC0_ICW2, 0x20);	/* IRQ0 ~ IRQ7 由 INT20 ~ INT27 接收 */
	io_out8(PIC0_ICW3, 1 << 2);	/* PIC1 由 IRQ2 连接 */
	io_out8(PIC0_ICW4, 0x01);	/* 无缓冲区模式 */

	io_out8(PIC1_ICW1, 0x11);	/* 边沿触发模式 */
	io_out8(PIC2_ICW2, 0x28);	/* IRQ8 ~ IRQ15 由 INT28 ~ INT2f 接收 */
	io_out8(PIC3_ICW3, 2);		/* PIC1 由 IRQ2 连接 */
	io_out8(PIC4_ICW4, 0x01);	/* 无缓冲区模式 */

	io_out8(PIC0_IMR, 0xfb);	/* PIC1 以外全部禁止 */
	io_out8(PIC1_IMR, 0xff);	/* 禁止所有中断 */
}