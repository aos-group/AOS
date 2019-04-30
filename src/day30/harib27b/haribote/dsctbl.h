#ifndef __DSCTBL_H
#define __DSCTBL_H

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define ADR_DATA		0x00000000
#define LIMIT_DATA		0xffffffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_LDT			0x0082
#define AR_INTGATE		0x008e
#define AR_TSS32		0x0089

#define AR_CODE32_ER_USER	AR_CODE32_ER + 0x60
#define AR_DATA32_RW_USER	AR_DATA32_RW + 0x60
#define AR_INTGATE_USER		AR_INTGATE + 0x60

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
