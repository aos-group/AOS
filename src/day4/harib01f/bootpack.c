void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

/**
 * HariMain - 启动函数
 */
void HariMain(void) {
	int i;
	char* p = (char*)0xa0000;

	init_palette();

	for (i = 0; i <= 0xffff; i++)
		p[i] = i & 0x0f;	/* 使用调色板 */

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
