#ifndef __SHEET_H
#define __SHEET_H

#include "memory.h"

#define MAX_SHEETS		256
#define SHEET_USE		1
#define SHEET_UNUSE		0

/**
 * SHEET - 图层
 * @buf: 图层上所描画内容的地址
 * @bxsize: @bysize: 图层的大小
 * @vx0: @vy0: 图层在画面上的位置的坐标
 * @col_inv: 透明色色号
 * @height: 图层的高度
 * @flags: 图层的设定信息
 */
struct SHEET {
	unsigned char* buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL* ctl;
};

/**
 * SHTCTL - 图层管理
 * @vram: @xsize: @ysize: VRAM 的地址和画面的大小
 * @top: 最上面图层的高度
 * @sheets0: 存放图层的信息
 * @sheets: 将 sheets0 按照高度升序排序
 */
struct SHTCTL {
	unsigned char* vram;
	int xsize, ysize, top;
	struct SHEET* sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL* shtctl_init(struct MEMMAN* memman, unsigned char* vram, int xsize, int ysize);
struct SHEET* sheet_alloc(struct SHTCTL* ctl);
void sheet_setbuf(struct SHEET* sht, unsigned char* buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET* sht, int height);
void sheet_refresh(struct SHEET* sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET* sht, int vx0, int vy0);
void sheet_free(struct SHEET* sht);
void sheet_refreshsub(struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0);

#endif
