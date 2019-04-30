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
 * @ctl: 图层控制器
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
 * @ctl: 图层控制器
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
 * @ctl: 图层控制器
 * @sht: 待操作的图层
 */
void sheet_free(struct SHEET* sht) {
	if (sht->height >= 0)
		sheet_updown(sht, -1);
	sht->flags = SHEET_UNUSE;
}

/******************************** 改进开始 *************************************/

/**
 * sheet_refreshsub - 对照着 map 指定范围刷新部分图层
 * @ctl: 图层控制器
 * @vx0: @vy0: 范围的左上角坐标
 * @vx1: @vy1: 范围的右下角坐标
 * @h0: 需要刷新的起始层数
 * @h1: 需要刷新的结束层数
 */
void sheet_refreshsub(struct SHTCTL* ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1) {
	int bx, by, vx, vy;
	unsigned char* buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET* sht;
	/* 如果 refresh 的范围超出了画面则修正 */
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	if (vy1 > ctl->ysize) vy1 = ctl->ysize;
	for (vx = vx0; vx < vx1; vx++) {
		for (vy = vy0; vy < vy1; vy++) {
			sid = map[vy * ctl->xsize + vx];
			sht = ctl->sheets0 + sid;
			if (h0 <= sht->height && sht->height <= h1) {
				bx = vx - sht->vx0;
				by = vy - sht->vy0;
				buf = sht->buf;
				vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
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
	int h, bx, by, vx, vy;
	unsigned char* buf, sid, *map = ctl->map;
	struct SHEET* sht;
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	if (vy1 > ctl->ysize) vy1 = ctl->ysize;
	for (vx = vx0; vx < vx1; vx++) {
		for (vy = vy0; vy < vy1; vy++) {
			for (h = ctl->top; h >= h0; h--) {
				sht = ctl->sheets[h];
				bx = vx - sht->vx0;
				by = vy - sht->vy0;
				if (0 <= bx && bx < sht->bxsize && 0 <= by && by < sht->bysize) {
					buf = sht->buf;
					sid = sht - ctl->sheets0;
					if (buf[by * sht->bxsize + bx] != sht->col_inv) {
						map[vy * ctl->xsize + vx] = sid;
						break;
					}
					/* 如果透明，还需要继续向下找 */
				}
			}
		}
	}
}

/******************************** 改进结束 *************************************/
