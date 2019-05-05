#include "apilib.h"
#include <stdio.h>
#define sprintf _sprintf
#define strtol _strtol

#define INVALID -0x7fffffff

/**
 * strtol - 将字符串转换为整形数
 * @s: 要转换的字符串
 * @endp: 第一个不能转换的字符的指针，由函数返回
 * @base: 字符串所采用的进制
 */
int strtol(char* s, char** endp, int base);		/* 标准函数(stdlib.h) */

char* skipspace(char* p);
int getnum(char** pp, int priority);

void _HariMain(void) {
	int i;
	char s[30], *p;

	api_cmdline(s, 30);
	for (p = s; *p > ' '; p++);		/* 一直读到空格为止 */
	i = getnum(&p, 9);
	if (i == INVALID) {
		api_putstr0("error!\n");
	} else {
		sprintf(s, "= %d = 0x%x\n", i, i);
		api_putstr0(s);
	}
	api_end();
}

char* skipspace(char* p) {
	for (; *p == ' '; p++);
	return p;
}

/**
 * getnum - 将字符串形式的算式进行解释，并获取一个数值。
 * @pp: 下一个字符地址(可能是空格)，由函数返回
 * @priority: 计算到哪个等级的运算符
 * 			  0: 单项运算符
 *			  1: *, /, %
 *			  2: +, -
 * 			  3: <<, >>
 * 			  4: &
 *			  5: ^
 * 			  6: |
 *			  9: ()  
 */
int getnum(char** pp, int priority) {
	char* p = *pp;
	int i = INVALID, j;
	p = skipspace(p);

	/* 单项运算符 */
	if (*p == '+') {
		p++;
		i = getnum(&p, 0);
	} else if (*p == '-') {
		p++;
		i = getnum(&p, 0);
		if (i != INVALID) {
			i = -i;
		}
	} else if (*p == '~') {
		p++;
		i = getnum(&p, 0);
		if (i != INVALID) {
			i = ~i;
		}
	} else if (*p == '(') {
		p++;
		i = getnum(&p, 9);
		p = skipspace(p);
		if (*p == ')') {
			p++;
		} else {
			i = INVALID;
		}
	} else if ('0' <= *p && *p <= '9') {
		i = strtol(p, &p, 0);	/* 自动识别 */
	}

	/* 二项运算符 */
	while (i != INVALID) {
		p = skipspace(p);
		if (*p == '+' && priority > 2) {
			p++;
			j = getnum(&p, 2);
			if (j != INVALID) {
				i += j;
			} else {
				i = INVALID;
			}
		} else if (*p == '-' && priority > 2) {
			p++;
			j = getnum(&p, 2);
			if (j != INVALID) {
				i -= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '*' && priority > 1) {
			p++;
			j = getnum(&p, 1);
			if (j != INVALID) {
				i *= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '/' && priority > 1) {
			p++;
			j = getnum(&p, 1);
			if (j != INVALID && j != 0) {
				i /= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '%' && priority > 1) {
			p++;
			j = getnum(&p, 1);
			if (j != INVALID && j != 0) {
				i %= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '<' && p[1] == '<' && priority > 3) {
			p += 2;
			j = getnum(&p, 3);
			if (j != INVALID && j != 0) {
				i <<= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '>' && p[1] == '>' && priority > 3) {
			p += 2;
			j = getnum(&p, 3);
			if (j != INVALID && j != 0) {
				i >>= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '&' && priority > 4) {
			p++;
			j = getnum(&p, 4);
			if (j != INVALID) {
				i &= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '^' && priority > 5) {
			p++;
			j = getnum(&p, 5);
			if (j != INVALID) {
				i ^= j;
			} else {
				i = INVALID;
			}
		} else if (*p == '|' && priority > 6) {
			p++;
			j = getnum(&p, 6);
			if (j != INVALID) {
				i |= j;
			} else {
				i = INVALID;
			}
		} else {
			break;
		}
	}
	*pp = p;
	return i;
}
