#include "int.h"
#include "asmhead.h"
#include "naskfunc.h"
#include "graphic.h"

void init_pic(void) {
	io_out8(PIC0_IMR, 0xff);	/* 禁止所有中断 */
	io_out8(PIC1_IMR, 0xff);	/* 禁止所有中断 */

	io_out8(PIC0_ICW1, 0x11);	/* 边沿触发模式 */
	io_out8(PIC0_ICW2, 0x20);	/* IRQ0 ~ IRQ7 由 INT20 ~ INT27 接收 */
	io_out8(PIC0_ICW3, 1 << 2);	/* PIC1 由 IRQ2 连接 */
	io_out8(PIC0_ICW4, 0x01);	/* 无缓冲区模式 */

	io_out8(PIC1_ICW1, 0x11);	/* 边沿触发模式 */
	io_out8(PIC1_ICW2, 0x28);	/* IRQ8 ~ IRQ15 由 INT28 ~ INT2f 接收 */
	io_out8(PIC1_ICW3, 2);		/* PIC1 由 IRQ2 连接 */
	io_out8(PIC1_ICW4, 0x01);	/* 无缓冲区模式 */

	io_out8(PIC0_IMR, 0xfb);	/* PIC1 以外全部禁止 */
	io_out8(PIC1_IMR, 0xff);	/* 禁止所有中断 */
}

/**
 * inthandler21 - 处理 0x21 号(PS/2 键盘)中断
 * @esp: 
 */
void inthandler21(int *esp) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 (IRQ-1) : PS/2 keyboard");
	while (1)
		io_hlt();
}

/**
 * inthandler2c - 处理 0x2c 号(PS/2 鼠标)中断
 * @esp: 
 */
void inthandler2c(int *esp) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2c (IRQ-12) : PS/2 mouse");
	while (1)
		io_hlt();
}

/**
 * inthandler27 - 来自 PIC0 的不完整中断处理
 * 在 Athlon 64 X 2 机器等中，由于芯片组的方便，在初始化 PIC 时只发生一次该中断
 * 这个中断处理函数会覆盖中断而不做任何事情
 * 这种中断是由 PIC 初始化时的电噪声引起的，不需要处理
 * 
 * @esp: 
 */
void inthandler27(int *esp) {
	/* |R|SL|EOI|0|0|L2|L1|L0| */
	/* |0| 1|  1|0|0| 1| 1| 1| */
	/* 特殊 EOI 结束方式： */
	/* 当中断处理完成后，向 8259A 发送 EOI 命令，8259A 将 ISR 寄存器中由 L2 ~ L1 指定的位清 0 */
	io_out8(PIC0_OCW2, 0x67);
}
