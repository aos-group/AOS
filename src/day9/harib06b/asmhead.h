#ifndef __ASMHEAD_H
#define __ASMHEAD_H

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char* vram;
};

#define ADR_BOOTINFO	0x00000ff0

#endif
