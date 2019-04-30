#include <stdio.h>

void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* vram, int x, int y);
void putfont8(char* vram, int xsize, int x, int y, char c, char* font);
void putfonts8_asc(char* vram, int xsize, int x, int y, char c, unsigned char* s);
void init_mouse_cursor8(char* mouse, char bc);
void putblock8_8(char* vram, int vxsize, int pxsize, int pysize, int px0, int py0, char* buf, int bxsize);

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

struct BOOTINFO {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char* vram;
};

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	struct BOOTINFO* binfo = (struct BOOTINFO*)0x0ff0;
	char s[40], mcursor[16 * 16];
	int mx = (binfo->scrnx - 16) >> 1;
	int my = (binfo->scrny - 28 - 16) >> 1;

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	while (1)
		io_hlt();
}

/**
 * init_palette - 初始化调色板
 */
void init_palette(void) {
	static unsigned char table_rgb[16 * 3] = {
		/*r 	g	  b*/
		0x00, 0x00, 0x00,	/*  0: 黒 */
		0xff, 0x00, 0x00,	/*  1: 亮红 */
		0x00, 0xff, 0x00,	/*  2: 亮绿 */
		0xff, 0xff, 0x00,	/*  3: 亮黄 */
		0x00, 0x00, 0xff,	/*  4: 亮蓝 */
		0xff, 0x00, 0xff,	/*  5: 亮紫 */
		0x00, 0xff, 0xff,	/*  6: 浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7: 白 */
		0xc6, 0xc6, 0xc6,	/*  8: 亮灰 */
		0x84, 0x00, 0x00,	/*  9: 暗红 */
		0x00, 0x84, 0x00,	/* 10: 暗绿 */
		0x84, 0x84, 0x00,	/* 11: 暗黄 */
		0x00, 0x00, 0x84,	/* 12: 暗青 */
		0x84, 0x00, 0x84,	/* 13: 暗紫 */
		0x00, 0x84, 0x84,	/* 14: 浅暗蓝 */
		0x84, 0x84, 0x84	/* 15: 暗灰 */
	};
	set_palette(0, 15, table_rgb);
}

/**
 * set_palette - 将编号与颜色写入调色板
 * @start: 起始编号
 * @end: 终止编号
 * @rgb: 调色表
 */
void set_palette(int start, int end, unsigned char *rgb) {
	int i, eflags;
	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		/* 除以4: 要把 0~255 映射到 0~63，因为那个 mode 下显卡的颜色标准就是 0~63 */
		io_out8(0x03c9, rgb[0] >> 2);
		io_out8(0x03c9, rgb[1] >> 2);
		io_out8(0x03c9, rgb[2] >> 2);
		rgb += 3;
	}
	io_store_eflags(eflags);
}

/**
 * boxfill8 - 用 8 位的颜色填充矩形
 * @vram: 表示像素的一维数组
 * @xsize: 水平方向像素量
 * @c: 填充颜色
 * @x0: @y0: @x1: @y1: 矩形对角坐标
 */
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1) {
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) 
			vram[y * xsize + x] = c;
	}
}

/**
 * init_screen - 初始化屏幕
 * @vram: 表示像素的一维数组
 * @x: 水平方向像素量
 * @y: 垂直方向像素量
 */
void init_screen(char* vram, int x, int y) {
	/* 垂直切分 */
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	/* 左侧任务条 */
	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	/* 右侧任务条 */
	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
}

/**
 * putfont8 - 将字符显示在屏幕上 (8 位颜色)
 * @vram: 表示像素的一维数组
 * @xsize: 水平方向像素量
 * @x: @y: 字符显示坐标
 * @c: 字符颜色
 * @font: 表示字符像素的数组
 */
void putfont8(char* vram, int xsize, int x, int y, char c, char* font) {
	int i, j;
	char* p, d;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		for (j = 7; j >= 0 && d; d >>= 1, j--)
			p[j] = (d & 1) ? c : p[j];
	}
}

/**
 * putfonts8_asc - 将字符串显示在屏幕上 (8 位颜色)
 * @vram: 表示像素的一维数组
 * @xsize: 水平方向像素量
 * @x: @y: 字符串起始坐标
 * @c: 字符颜色
 * @s: 待显示的字符串
 */
void putfonts8_asc(char* vram, int xsize, int x, int y, char c, unsigned char* s) {
	extern char hankaku[4096];
	for (; *s; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
}

/**
 * init_mouse_cursor8 - 获取鼠标指针 (8 位颜色)
 * @mouse: 鼠标指针像素数组
 * @bc: 鼠标背景色
 */
void init_mouse_cursor8(char* mouse, char bc) {
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			} else if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			} else if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
}

/**
 * putblock8_8 - 填充屏幕上的区块 (8 位颜色)
 * @vram: 表示像素的一维数组
 * @vxsize: 屏幕水平方向像素量
 * @pxsize: 待显示图形的水平方向像素量
 * @pysize: 待显示图形的垂直方向像素量
 * @px0: @py0: 指定图形的显示位置
 * @buf: 指定图形的存放地址
 * @bxsize: 指定图形每一行含有的像素数
 */
void putblock8_8(char* vram, int vxsize, int pxsize, int pysize, int px0, int py0, char* buf, int bxsize) {
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
}
