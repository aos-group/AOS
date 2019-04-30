#include <stdio.h>
#include <string.h>
#include "apilib.h"

void putstr(int win, char* winbuf, int x, int y, int col, unsigned char* s);
void wait(int i, int timer, char* keyflag);

static unsigned char charset[16 * 8] = {
	/* invader(0) */
	0x00, 0x00, 0x00, 0x43, 0x5f, 0x5f, 0x5f, 0x7f,
	0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x20, 0x3f, 0x00,

	/* invader(1) */
	0x00, 0x0f, 0x7f, 0xff, 0xcf, 0xcf, 0xcf, 0xff,
	0xff, 0xe0, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0x00,

	/* invader(2) */
	0x00, 0xf0, 0xfe, 0xff, 0xf3, 0xf3, 0xf3, 0xff,
	0xff, 0x07, 0xff, 0xff, 0x03, 0x03, 0x03, 0x00,

	/* invader(3) */
	0x00, 0x00, 0x00, 0xc2, 0xfa, 0xfa, 0xfa, 0xfe,
	0xf8, 0xf8, 0xf8, 0xf8, 0x00, 0x04, 0xfc, 0x00,

	/* fighter(0) */
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x43, 0x47, 0x4f, 0x5f, 0x7f, 0x7f, 0x00,

	/* fighter(1) */
	0x18, 0x7e, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,
	0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0x00,

	/* fighter(2) */
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0xc2, 0xe2, 0xf2, 0xfa, 0xfe, 0xfe, 0x00,

	/* laser */
	0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00
};
/* invader:"abcd", fighter:"efg", laser:"h" */

void HariMain(void) {
	int win, timer, i, j;
	int laserwait;	/* 等离子炮弹的剩余充电时间 */
	int fx;		/* 自机的 x 坐标 */
	int lx = 0, ly;		/* 等离子炮弹的坐标 */
	int ix, iy;		/* 外星人群的坐标 */
	int movewait0;	/* movewait 的初始值 (消灭 30 只敌人后减少) */
	int movewait;	/* 当这个变量变为 0 时外星人群前进一步 */
	int idir;	/* 外星人群的移动方向 */
	int invline;	/* 外星人群的行数 */
	int score;	/* 当前得分 */
	int high;	/* 最高得分 */
	int point;	/* 得分的增加量 */
	char winbuf[336 * 261], s[12], keyflag[4], *p;
	char invstr[32 * 6];	/* 将外星人群的状态显示为字符串的变量 */ 
	static char invstr0[32] = " abcd abcd abcd abcd abcd ";		/* len: 26 */

	win = api_openwin(winbuf, 336, 261, -1, "invader");
	api_boxfilwin(win, 6, 27, 329, 254, 0);		/* 40 * 14 */
	timer = api_alloctimer();
	api_inittimer(timer, 128);

	high = 0;
	putstr(win, winbuf, 22, 0, 7, "HIGH:00000000");

restart:	/* 游戏重启 */
	score = 0;
	point = 1;
	putstr(win, winbuf, 4, 0, 7, "SCORE:00000000");
	movewait0 = 20;
	fx = 18;
	putstr(win, winbuf, fx, 13, 6, "efg");
	wait(100, timer, keyflag);

next_group:		/* 下一关 */
	wait(100, timer, keyflag);
	ix = 7;
	iy = 1;
	invline = 6;
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 27; j++) {		/* 最后加 '\0' */
			invstr[i * 32 + j] = invstr0[j];
		}
		putstr(win, winbuf, ix, iy + i, 2, invstr + i * 32);
	}
	keyflag[0] = 0;
	keyflag[1] = 0;
	keyflag[2] = 0;

	ly = 0;		/* 不显示 */
	laserwait = 0;
	movewait = movewait0;
	idir = 1;	/* 右 */
	wait(100, timer, keyflag);

	while (1) {
		if (laserwait != 0) {
			laserwait--;
			keyflag[2 /* space */] = 0;
		}

		wait(4, timer, keyflag);

		/* 自机的处理 */
		if (keyflag[0 /* left */] != 0 && fx > 0) {
			fx--;
			putstr(win, winbuf, fx, 13, 6, "efg ");
			keyflag[0 /* left */] = 0;
		} else if (keyflag[1 /* right */] != 0 && fx < 37) {
			putstr(win, winbuf, fx, 13, 6, " efg");
			fx++;
			keyflag[1 /* right */] = 0;
		} else if (keyflag[2 /* space */] != 0 && laserwait == 0) {
			laserwait = 15;
			lx = fx + 1;
			ly = 13;
		}

		/* 外星人移动 */
		if (movewait != 0) {
			movewait--;
		} else {
			movewait = movewait0;
			/* 			  40 - 26 */
			if (ix + idir > 14 || ix + idir < 0) {
				if (iy + invline == 13)
					break;	/* GAME OVER */
				idir = -idir;
				/* 				  第一个为空格 */
				putstr(win, winbuf, ix + 1, iy, 0, "                         ");	/* 清除最上面一行 */
				iy++;
			} else {
				ix += idir;
			}
			for (i = 0; i < invline; i++) {
				putstr(win, winbuf, ix, iy + i, 2, invstr + i * 32);
			}
		}

		/* 炮弹处理 */
		if (ly > 0) {
			if (ly < 13) {
				/* 删去原来位置上的内容 */
				if (ix < lx && lx < ix + 25 && iy <= ly && ly < iy + invline) {
					/* 在敌人矩形中，炮弹将去下一格，重新描画炮弹所在行 */
					putstr(win, winbuf, ix, ly, 2, invstr + (ly - iy) * 32);
				} else {
					putstr(win, winbuf, lx, ly, 0, " ");
				}
			}
			/* 重新描绘 */
			ly--;
			if (ly > 0) {
				putstr(win, winbuf, lx, ly, 3, "h");
			} else {
				point -= 10;
				if (point <= 0) {
					point = 1;
				}
			}
			if (ix < lx && lx < ix + 25 && iy <= ly && ly < iy + invline) {
				/* 在敌人矩形中 */
				p = invstr + (ly - iy) * 32 + (lx - ix);
				if (*p != ' ') {
					/* hit! */
					/* 处理分数 */
					score += point;
					point++;
					sprintf(s, "%08d", score);
					putstr(win, winbuf, 10, 0, 7, s);
					if (high < score) {
						high = score;
						putstr(win, winbuf, 27, 0, 7, s);
					}
					/* 处理画面 */
					for (p--; *p != ' '; p--);
					for (i = 1; i < 5; i++) {
						p[i] = ' ';
					}
					putstr(win, winbuf, ix, ly, 2, invstr + (ly - iy) * 32);
					for (; invline > 0; invline--) {
						for (p = invstr + (invline - 1) * 32; *p; p++) {
							if (*p != ' ') {
								goto alive;
							}
						}
					}
					/* 全部消失 */
					movewait0 -= movewait0 / 3;
					goto next_group;
alive:
					ly = 0;
				}
			}
		}
	}
	/* GAME OVER */
	putstr(win, winbuf, 15, 6, 1, "GAME OVER");
	wait(0, timer, keyflag);
	for (i = 1; i < 14; i++) {
		putstr(win, winbuf, 0, i, 0, "                                        ");
	}
	goto restart;
}

/**
 * putstr - 显示字符串，不过 a~h 的字符不是直接显示，而是用 charset 的字库来显示的
 */
void putstr(int win, char* winbuf, int x, int y, int col, unsigned char* s) {
	int c, x0, i, j;
	char* p, *q, t[2], d;
	x = x * 8 + 8;
	y = y * 16 + 29;
	x0 = x;		/* 起始列数 */
	i = strlen(s);
	api_boxfilwin(win + 1, x, y, x + i * 8 - 1, y + 15, 0);
	q = winbuf + y * 336;	/* 显示的行 */
	t[1] = 0;
	for (; *s; s++) {
		c = *s;
		if (c != ' ') {
			if ('a' <= c && c <= 'h') {
				p = charset + 16 * (c - 'a');
				q += x;
				for (i = 0; i < 16; i++) {
					d = p[i];
					for (j = 7; j >= 0 && d; d >>= 1, j--)
						q[j] = (d & 1) ? col : q[j];
					q += 336;
				}
				q -= 336 * 16 + x;
			} else {
				t[0] = *s;
				api_putstrwin(win + 1, x, y, col, 1, t);
			}
		}
		x += 8;
	}
	api_refreshwin(win, x0, y, x, y + 16);
}

/**
 * wait - 延时并等待按键输入
 * @i: 0 等待回车键的输入
 * 	   否则按照"指定的时间 * 0.01s"为基准进行延时等待，在等待期间如果有按键输入则反映到 keyflag[0~2] 中
 */
void wait(int i, int timer, char* keyflag) {
	int j;
	if (i > 0) {
		/* 等待一段时间 */
		api_settimer(timer, i);
		i = 128;
	} else {
		i = 0x0a;	/* Enter */
	}
	while (1) {
		j = api_getkey(1);
		if (j == i) {
			break;
		} else if (j == '4') {
			keyflag[0 /* left */] = 1;
		} else if (j == '6') {
			keyflag[1 /* right */] = 1;
		} else if (j == ' ') {
			keyflag[2 /* space */] = 1;
		}
	}
}
