#include "sheet.h"

struct SHTCTL* shtctl_init(struct MEMMAN* man, unsigned char* vram, int xsize, int ysize) {
	struct SHTCTL* ctl;
	int i;
	ctl = (struct SHTCTL*)memman_alloc_4k(man, sizeof(struct SHTCTL));
	if (ctl == 0)
		goto err;
	ctl->map = (unsigned char*)memman_alloc_4k(man, xsize * ysize);
	if (ctl->map == 0) {
		memman_free_4k(man, (int)ctl, sizeof(struct SHTCTL));
		goto err;
	}
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
			sht->task = 0;		/* 不使用自动关闭功能 */
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
			/* 不刷新 height 层的原因：图层下降只会被覆盖更多 */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} else {	/* 隐藏 */
			if (ctl->top > old) {
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
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
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}
}

/**
 * sheet_refresh - 指定缓冲区内的坐标范围刷新指定图层
 * 由于图层的上下关系没有改变，所以不需要重新进行 sheet_refreshmap 的处理
 * 同理，其他图层没有改变，对于该图层，该被遮住的还是会被遮住，因此只需刷新该层就可以了
 *
 * @sht: 待操作的图层
 * @bx0: @by0: 缓冲区内的左上角坐标
 * @bx1: @by1: 缓冲区内的右下角坐标
 */
void sheet_refresh(struct SHEET* sht, int bx0, int by0, int bx1, int by1) {
	if (sht->height >= 0)
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
}

/**
 * sheet_slide - 移动图层
 * @sht: 待操作的图层
 * @vx0: @vy0: 移动后的坐标
 */
void sheet_slide(struct SHEET* sht, int vx0, int vy0) {
	struct SHTCTL* ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
}

/**
 * sheet_free - 删去图层，并释放该图层
 * @sht: 待操作的图层
 */
void sheet_free(struct SHEET* sht) {
	if (sht->height >= 0)
		sheet_updown(sht, -1);
	sht->flags = SHEET_UNUSE;
}

/**
 * sheet_refreshsub - 对照着 map 指定范围刷新部分图层
 * @ctl: 图层控制器
 * @vx0: @vy0: 范围的左上角坐标
 * @vx1: @vy1: 范围的右下角坐标
 * @h0: 需要刷新的起始层数
 * @h1: 需要刷新的结束层数
 */
void sheet_refreshsub(struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1, bx2, sid4, i, i1, *p, *q, *r, j;
	unsigned char* buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET* sht;
	/* 如果 refresh 的范围超出了画面则修正 */
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	if (vy1 > ctl->ysize) vy1 = ctl->ysize;
	for (h = h0; h <= h1; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) bx0 = 0;
		if (by0 < 0) by0 = 0;
		if (bx1 > sht->bxsize) bx1 = sht->bxsize;
		if (by1 > sht->bysize) by1 = sht->bysize;
		if ((sht->vx0 & 3) == 0) {
			/* 4 字节型 */
			i = (bx0 + 3) >> 2;		/* bx0 / 4 (小数进位) */
			i1 = bx1 >> 2;		/* bx1 / 4 (小数舍去) */
			i1 = i1 - i;
			sid4 = sid | sid << 8 | sid << 16 | sid << 24;
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1 && (bx & 3) != 0; bx++) {	/* 前面被 4 除多余的部分逐个字节写入 */
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid)		/* 对照着 map 的内容向 VRAM 中写入 */
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
				vx = sht->vx0 + bx;
				p = (int*)&map[vy * ctl->xsize + vx];
				q = (int*)&vram[vy * ctl->xsize + vx];
				r = (int*)&buf[by * sht->bxsize + bx];
				for (i = 0; i < i1; i++) {		/* 4 的倍数部分 */
					if (p[i] == sid4) {
						q[i] = r[i];	/* 估计大多数会是这种情况，因此速度会更快 */
					} else {
						bx2 = bx + (i << 2);
						vx = sht->vx0 + bx2;
						for (j = 0; j < 4; j++) {
							if (map[vy * ctl->xsize + vx + j] == sid)
								vram[vy * ctl->xsize + vx + j] = buf[by * sht->bxsize + bx2 + j];
						}
					}
				}
				for (bx += (i1 << 2); bx < bx1; bx++) {		/* 后面被 4 除多余的部分逐个字节写入 */
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid)		/* 对照着 map 的内容向 VRAM 中写入 */
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		} else {
			/* 1 字节型 */
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid)		/* 对照着 map 的内容向 VRAM 中写入 */
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
}

/**
 * sheet_refreshmap - 更新画面上的点是哪个图层的像素
 * @ctl: 图层控制器
 * @vx0: @vy0: 范围的左上角坐标
 * @vx1: @vy1: 范围的右下角坐标
 * @h0: 需要刷新的起始层数
 */
void sheet_refreshmap(struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1, sid4, *p;
	unsigned char* buf, sid, *map = ctl->map;
	struct SHEET* sht;
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	if (vy1 > ctl->ysize) vy1 = ctl->ysize;
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;		/* 将进行了减法计算的地址作为图层号码使用 */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) bx0 = 0;
		if (by0 < 0) by0 = 0;
		if (bx1 > sht->bxsize) bx1 = sht->bxsize;
		if (by1 > sht->bysize) by1 = sht->bysize;
		if (sht->col_inv == -1) {
			/* 无透明色图层专用的高速版 */
			if ((sht->vx0 & 3) == 0 && (bx0 & 3) == 0 && (bx1 & 3) == 0) {
				/* 4 字节型 */
				bx1 = (bx1 - bx0) >> 2;		/* mov 次数 */
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					vx = sht->vx0 + bx0;
					p = (int*)&map[vy * ctl->xsize + vx];
					for (bx = 0; bx < bx1; bx++)
						p[bx] = sid4;
				}
			} else {
				/* 1 字节型 */
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					for (bx = bx0; bx < bx1; bx++) {
						vx = sht->vx0 + bx;
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		} else {
			/* 有透明色图层用的普通版 */
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (buf[by * sht->bxsize + bx] != sht->col_inv)
						map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
}
