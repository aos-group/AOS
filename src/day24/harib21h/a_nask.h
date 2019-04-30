#ifndef __A_NASK_H
#define __A_NASK_H

void api_putchar(int c);
void api_end(void);
void api_putstr0(char* s);
int api_openwin(char* buf, int xsize, int ysize, int col_inv, char* title);
void api_putstrwin(int win, int x, int y, int col, int len, char* str);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char* api_malloc(int size);
void api_free(char* addr, int size);
void api_point(int win, int x, int y, int col);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
void api_closewin(int win);
int api_getkey(int mode);
int api_alloctimer(void);
void api_inittimer(int timer, int data);
void api_settimer(int timer, int time);
void api_freetimer(int timer);

#endif
