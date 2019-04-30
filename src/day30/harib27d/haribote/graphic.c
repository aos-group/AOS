/* 图形处理 */
#include "graphic.h"
#include "naskfunc.h"
#include "mtask.h"

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
	unsigned char table2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {	/* 每种颜色 6 个色阶 */
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;	/* 255 / 5 */
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	set_palette(16, 231, table2);	/* 共 6*6*6=216 种颜色 */
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

void putfont_chinese(char* vram, int xsize, int x, int y, char c, char* font) {
	int i, j, k;
	char* p, d;
	for (i = 0; i < 16; i++) {
		for (k = 0; k < 2; k++) {
			p = vram + (y + i) * xsize + x + 8 * k;
			d = font[i * 2 + k];
			for (j = 7; j >= 0 && d; d >>= 1, j--)
				p[j] = (d & 1) ? c : p[j];
		}
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
	struct TASK* task = task_now();
	extern char* nihongo, *chinese;
	char* font;
	int k;	/* 区号，存放的是减 1 之后的值 */
	int t;	/* 点号，存放的是减 1 之后的值 */
	if (task->langmode == 0) {
		for (; *s; s++) {
			putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
			x += 8;
		}
	} else if (task->langmode == 1) {
		for (; *s; s++) {
			if (task->langbyte1 == 0) {
				if ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)) {		/* 全角字符第 1 字节 */
					task->langbyte1 = *s;
				} else {	/* 半角字符 */
					putfont8(vram, xsize, x, y, c, nihongo + *s * 16);
				}
			} else {
				if (0x81 <= task->langbyte1 && task->langbyte1 <= 0x9f) {	/* 第 1 部分 */
					k = (task->langbyte1 - 0x81) * 2;
				} else {	/* 第 2 部分 */
					k = (task->langbyte1 - 0xe0) * 2 + 62;
				}
				/* 全角字符第 2 字节 */
				if (0x40 <= *s && *s <= 0x7e) {
					t = *s - 0x40;
				} else if (0x80 <= *s && *s <= 0x9e) {
					t = *s - 0x80 + 63;
				} else {
					t = *s - 0x9f;
					k++;
				}
				task->langbyte1 = 0;
				font = nihongo + 256 * 16 + (k * 94 + t) * 32;
				putfont8(vram, xsize, x - 8, y, c, font);	/* 左半部分 */
				putfont8(vram, xsize, x, y, c, font + 16);	/* 右半部分 */
			}
			x += 8;
		}
	} else if (task->langmode == 2) {
		for (; *s; s++) {
			if (task->langbyte1 == 0) {
				if (0x81 <= *s && *s <= 0xfe) {
					task->langbyte1 = *s;
				} else {
					putfont8(vram, xsize, x, y, c, nihongo + *s * 16);
				}
			} else {
				k = task->langbyte1 - 0xa1;
				t = *s - 0xa1;
				task->langbyte1 = 0;
				font = nihongo + 256 * 16 + (k * 94 + t) * 32;
				putfont8(vram, xsize, x - 8, y, c, font);	/* 左半部分 */
				putfont8(vram, xsize, x, y, c, font + 16);	/* 右半部分 */
			}
			x += 8;
		}
	} else if (task->langmode == 3) {
		for (; *s; s++) {
			if (task->langbyte1 == 0) {
				if (0xa1 <= *s && *s <= 0xfe) {
					task->langbyte1 = *s;
				} else {
					putfont8(vram, xsize, x, y, c, chinese + *s * 16);
				}
			} else {
				k = task->langbyte1 - 0xa1;
				t = *s - 0xa1;
				task->langbyte1 = 0;
				font = chinese + 256 * 16 + (k * 94 + t) * 32;
				putfont_chinese(vram, xsize, x - 8, y, c, font);
			}
			x += 8;
		}
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
