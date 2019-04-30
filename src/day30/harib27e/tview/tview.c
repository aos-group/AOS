#include "apilib.h"
#include <stdio.h>

/**
 * strtol - 将字符串转换为整形数
 * @s: 要转换的字符串
 * @endp: 第一个不能转换的字符的指针，由函数返回
 * @base: 字符串所采用的进制
 */
int strtol(char* s, char** endp, int base);		/* 标准函数(stdlib.h) */

char* skipspace(char* p);
void textview(int win, int w, int h, int xskip, char* p, int tab, int lang);
char* lineview(int win, int w, int y, int xskip, unsigned char* p, int tab, int lang);
int puttab(int x, int w, int xskip, char* s, int tab);

void HariMain(void) {
	char winbuf[1024 * 757];	/* 窗口缓冲区 */
	char txtbuf[240 * 1024];	/* 文件内容缓冲区 */
	int w = 30;		/* 列数 */
	int h = 10;		/* 行数 */
	int t = 4;		/* 制表符大小 */
	int spd_x = 1, spd_y = 1;	/* 滚动速度 */
	int win, i, j, lang = api_getlang();
	int xskip = 0;	/* 每行显示的字符相对于该行第一个字符的偏移量 */
	char s[30], *p;
	char *q = 0, *r = 0;	/* 文件名称字符串的起始和终止指针 */

	/* 命令行解析 */
	api_cmdline(s, 30);
	for (p = s; *p > ' '; p++);		/* 一直读到空格为止 */
	for (; *p; ) {
		p = skipspace(p);
		if (*p == '-') {
			if (p[1] == 'w') {
				w = strtol(p + 2, &p, 0);
				if (w < 20) {
					w = 20;
				} else if (w > 126) {
					w = 126;
				}
			} else if (p[1] == 'h') {
				h = strtol(p + 2, &p, 0);
				if (h < 1) {
					h = 1;
				} else if (h > 45) {
					h = 45;
				}
			} else if (p[1] == 't') {
				t = strtol(p + 2, &p, 0);
				if (t < 1) {
					t = 4;
				}
			} else {
err:
				api_putstr0(" >tview file [-w30 -h10 -t4]\n");
				api_end();
			}
		} else {	/* 找到文件名 */
			if (q != 0) {	/* 已经存在一个文件 */
				goto err;
			}
			q = p;
			for (; *p > ' '; p++);	/* 一直读到空格为止 */
			r = p;
		}
	}
	if (q == 0) {	/* 没有注明文件 */
		goto err;
	}

	/* 准备窗口 */
	win = api_openwin(winbuf, w * 8 + 16, h * 16 + 37, -1, "tview");
	api_boxfilwin(win, 6, 27, w * 8 + 9, h * 16 + 30, 7);

	/* 载入文件 */
	*r = 0;
	i = api_fopen(q);
	if (i == 0) {
		api_putstr0("file open error.\n");
		api_end();
	}
	j = api_fsize(i, 0);
	if (j >= 240 * 1024 - 1) {
		j = 240 * 1024 - 2;
	}
	txtbuf[0] = 0x0a;	/* 卫兵用的换行代码 */
	api_fread(txtbuf + 1, j, i);
	api_fclose(i);
	txtbuf[j + 1] = 0;
	q = txtbuf + 1;		/* 文件内容 */
	for (p = txtbuf + 1; *p; p++) {		/* 为了让处理变得简单，删掉 0x0d 的代码 */
		if (*p != 0x0d) {
			*q = *p;
			q++;
		}
	}
	*q = 0;

	/* 主体 */
	p = txtbuf + 1;
	while (1) {
		textview(win, w, h, xskip, p, t, lang);
		i = api_getkey(1);
		if (i == 'Q' || i == 'q') {
			api_end();
		} else if ('A' <= i && i <= 'F') {
			spd_x = 1 << (i - 'A');
		} else if ('a' <= i && i <= 'f') {
			spd_y = 1 << (i - 'a');
		} else if (i == '<' && t > 1) {
			t >>= 1;
		} else if (i == '>' && t < 256) {
			t <<= 1;
		} else if (i == '4') {
			while (1) {
				xskip -= spd_x;
				if (xskip < 0) {
					xskip = 0;
				}
				if (api_getkey(0) != '4') {		/* 如果没有按下 '4' 则处理结束 */
					break;
				}
			}
		} else if (i == '6') {
			while (1) {
				xskip += spd_x;
				if (api_getkey(0) != '6') {
					break;
				}
			}
		} else if (i == '8') {
			while (1) {
				for (j = 0; j < spd_y; j++) {
					if (p == txtbuf + 1) {
						break;
					}
					for (p--; p[-1] != 0x0a; p--);		/* 回溯到上一个字符为 0x0a 为止 */
				}
				if (api_getkey(0) != '8') {
					break;
				}
			}
		} else if (i == '2') {
			while (1) {
				for (j = 0; j < spd_y; j++) {
					for (q = p; *q != 0 && *q != 0x0a; q++);
					if (*q == 0) {
						break;
					}
					p = q + 1;
				}
				if (api_getkey(0) != '2') {
					break;
				}
			}
		}
	}
}

char* skipspace(char* p) {
	for (; *p == ' '; p++);
	return p;
}

void textview(int win, int w, int h, int xskip, char* p, int tab, int lang) {
	int i;
	api_boxfilwin(win + 1, 8, 29, w * 8 + 7, h * 16 + 28, 7);
	for (i = 0; i < h; i++) {
		p = lineview(win, w, i * 16 + 29, xskip, p, tab, lang);
	}
	api_refreshwin(win, 8, 29, w * 8 + 8, h * 16 + 29);
}

char* lineview(int win, int w, int y, int xskip, unsigned char* p, int tab, int lang) {
	int x = -xskip;
	char s[130];
	while (1) {
		if (*p == 0) {
			break;
		}
		if (*p == 0x0a) {
			p++;
			break;
		}
		if (*p == 0x09) {	/* 制表符 */
			x = puttab(x, w, xskip, s, tab);
			p++;
		} else if ((lang == 1 && ((0x81 <= *p && *p <= 0x9f) || (0xe0 <= *p && *p <= 0xfc)))	/* SJIS */
				|| (lang == 2 && (0xa1 <= *p && *p <= 0xfe))	/* EUC */
				|| (lang == 3 && (0xa1 <= *p && *p <= 0xfe))) {		/* chinese */
			/* 全角字符 */
			if (x == -1) {
				s[0] = ' ';		/* 左半边不显示，右半边也不显示 */
			} else if (0 <= x && x < w - 1) {
				s[x] = p[0];
				s[x + 1] = p[1];
			} else if (x == w - 1) {
				s[w - 1] = ' ';		/* 右半边不显示，左半边也不显示 */
			}
			x += 2;
			p += 2;
		} else {
			if (0 <= x && x < w) {
				s[x] = *p;
			}
			x++;
			p++;
		}
	}
	if (x > w) {
		x = w;
	}
	if (x > 0) {
		s[x] = 0;
		api_putstrwin(win + 1, 8, y, 0, x, s);
	}
	return p;
}

/**
 * puttab - 向 @s 中从 @x 位置处写入空格直至 @tab 对齐
 * @w: 最大写入数量
 * @return: 写入结束位置
 */
int puttab(int x, int w, int xskip, char* s, int tab) {
	while (1) {
		if (0 <= x && x < w) {
			s[x] = ' ';
		}
		x++;
		if ((x + xskip) % tab == 0) {
			break;
		}
	}
	return x;
}
