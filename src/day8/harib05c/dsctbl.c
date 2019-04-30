/* 关于 GDT, IDT 等 descriptor table 的处理 */
#include "dsctbl.h"
#include "naskfunc.h"

void init_gdtidt(void) {
	struct SEGMENT_DESCRIPTOR* gdt = (struct SEGMENT_DESCRIPTOR*)ADR_GDT;
	struct GATE_DESCRIPTOR* idt = (struct GATE_DESCRIPTOR*)ADR_IDT;
	int i;

	/* GDT 的初始化 */
	for (i = 0; i < 8192; i++)
		set_segmdesc(gdt + i, 0, 0, 0);
	set_segmdesc(gdt + 1, LIMIT_DATA, ADR_DATA, AR_DATA32_RW);	/* 读写数据段 */
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);	/* 一致性代码段 512KB */
	load_gdtr(LIMIT_GDT, ADR_GDT);

	/* IDT 的初始化 */
	for (i = 0; i < 256; i++)
		set_gatedesc(idt + i, 0, 0, 0);
	set_gatedesc(idt + 0x21, (int)asm_inthandler21, 2 << 3, AR_INTGATE);
	set_gatedesc(idt + 0x2c, (int)asm_inthandler2c, 2 << 3, AR_INTGATE);
	load_idtr(LIMIT_IDT, ADR_IDT);
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar) {
	if (limit > 0xfffff) {
		ar |= 0x8000;		/* G_bit = 1 */
		limit >>= 12;
	}
	sd->limit_low = limit & 0xffff;
	sd->base_low = base & 0xffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high = (base >> 24) & 0xff;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar) {
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = (offset >> 16) & 0xffff;
}
