#ifndef __DSCTBL_H
#define __DSCTBL_H

#define ADR_IDT			0x0026f800	/* IDT 的地址 */
#define LIMIT_IDT		0x000007ff	/* IDT 的界限，2KB, 256 项 */
#define ADR_GDT			0x00270000	/* GDT 的地址 */
#define LIMIT_GDT		0x0000ffff	/* GDT 的界限，64KB, 8192 项 */
#define ADR_BOTPAK		0x00280000	/* 内核代码段基址 */
#define LIMIT_BOTPAK	0x0007ffff	/* 内核代码段界限 */
#define ADR_DATA		0x00000000	/* 内核数据段基址 */
#define LIMIT_DATA		0xffffffff	/* 内核数据段界限 */
#define AR_DATA32_RW	0x4092		/* 内核数据段属性 */
#define AR_CODE32_ER	0x409a		/* 内核代码段属性 */
#define AR_LDT			0x0082 		/* LDT 属性，系统段(S = 0)，type = 0x2 */
#define AR_INTGATE		0x008e 		/* 内核中断门属性，系统段(S = 0)，type = 0xe */
#define AR_TSS32		0x0089 		/* TSS 属性，系统段(S = 0)，type = 0x9 */

#define AR_CODE32_ER_USER	AR_CODE32_ER + 0x60 	/* 用户代码段属性 */
#define AR_DATA32_RW_USER	AR_DATA32_RW + 0x60 	/* 用户数据段属性 */
#define AR_INTGATE_USER		AR_INTGATE + 0x60 		/* 用户中断门属性 */

struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

extern struct SEGMENT_DESCRIPTOR* gdt;
extern struct GATE_DESCRIPTOR* idt;

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif
