#include "sheet.h"

struct SHTCTL* shtctl_init(struct MEMMAN* memman, unsigned char* vram, int xsize, int ysize) {
	struct SHTCTL* ctl;
	int i;
	ctl = (struct SHTCTL*)memman_alloc_4k(memman, sizeof(struct SHTCTL));
	if (ctl == 0)
		goto err;
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;		/* 一个 SHEET 都没有 */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = SHEET_UNUSE;		/* 标记为未使用 */
		ctl->sheets0[i].ctl = ctl;
	}
err:
	return ctl;
}

struct SHEET* sheet_alloc(struct SHTCTL* ctl) {
	struct SHEET* sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == SHEET_UNUSE) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE;		/* 标记为正在使用 */
			sht->height = -1;		/* 隐藏 */
			return sht;
		}
	}
	return 0;	/* 所有的 SHEET 都处于正在使用状态 */
}

void sheet_setbuf(struct SHEET* sht, unsigned char* buf, int xsize, int ysize, int col_inv) {
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
}

/**
 * sheet_updown - 调整图层到指定高度
 * @ctl: 图层控制器
 * @sht: 待操作的图层
 * @height: 指定的高度
 */
void sheet_updown(struct SHEET* sht, int height) {
	struct SHTCTL* ctl = sht->ctl;
	int h, old = sht->height;	/* 设置前的高度信息 */

	/* 如果指定的高度过高或过低，则进行修正 */
	if (height > ctl->top + 1)
		height = ctl->top + 1;
	if (height < -1)
		height = -1;
	sht->height = height;

	/* 下面主要是进行 sheets 的重新排列 */
	if (old > height) {
		/* 比以前低 */
		if (height >= 0) {
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 隐藏 */
			if (ctl->top > old) {
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
		}
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {
		if (old >= 0) {
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 由隐藏状态转为显示状态 */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++;
		}
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
}

/**
 * sheet_refresh - 指定缓冲区内的坐标范围刷新所有图层
 * @ctl: 图层控制器
 * @sht: 待操作的图层
 * @bx0: @by0: 缓冲区内的左上角坐标
 * @bx1: @by1: 缓冲区内的右下角坐标
 */
void sheet_refresh(struct SHEET* sht, int bx0, int by0, int bx1, int by1) {
	if (sht->height >= 0)
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
}

/**
 * sheet_slide - 移动图层
 * @ctl: 图层控制器
 * @sht: 待操作的图层
 * @vx0: @vy0: 移动后的坐标
 */
void sheet_slide(struct SHEET* sht, int vx0, int vy0) {
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {
		sheet_refreshsub(sht->ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(sht->ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
}

/**
 * sheet_free - 删去图层，并释放该图层
 * @ctl: 图层控制器
 * @sht: 待操作的图层
 */
void sheet_free(struct SHEET* sht) {
	if (sht->height >= 0)
		sheet_updown(sht, -1);
	sht->flags = SHEET_UNUSE;
}

/**
 * sheet_refreshsub - 指定范围刷新所有图层
 * @ctl: 图层控制器
 * @vx0: @vy0: 范围的左上角坐标
 * @vx1: @vy1: 范围的右下角坐标
 */
void sheet_refreshsub(struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char* buf, c, *vram = ctl->vram;
	struct SHEET* sht;
	/* 如果 refresh 的范围超出了画面则修正 */
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	if (vy1 > ctl->ysize) vy1 = ctl->ysize;
	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) bx0 = 0;
		if (by0 < 0) by0 = 0;
		if (bx1 > sht->bxsize) bx1 = sht->bxsize;
		if (by1 > sht->bysize) by1 = sht->bysize;
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];
				if (c != sht->col_inv)
					vram[vy * ctl->xsize + vx] = c;
			}
		}
	}
}
