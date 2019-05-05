#ifndef __ASMHEAD_H
#define __ASMHEAD_H

#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000
#define KEYCMD_LED		0xed

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char* vram;
};

extern struct BOOTINFO* binfo;

#endif
