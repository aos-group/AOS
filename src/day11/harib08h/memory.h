#ifndef __MEMORY_H
#define __MEMORY_H

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
#define MEMMAN_FREES		4090			/* 大约是 32 KB */
#define MEMMAN_ADDR			0x003c0000

struct FREEINFO {
	unsigned int addr, size;
};

struct MEMMAN {
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);

/**
 * memtest_sub - 调查从 start 地址到 end 地址的范围内，能够使用的内存的末尾地址
 * @start: 内存起始地址
 * @end: 内存结束地址
 */
unsigned int memtest_sub(unsigned int start, unsigned int end);

void memman_init(struct MEMMAN* man);
unsigned int memman_total(struct MEMMAN* man);
unsigned int memman_alloc(struct MEMMAN* man, unsigned int size);
int memman_free(struct MEMMAN* man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN* man, unsigned int size);
int memman_free_4k(struct MEMMAN* man, unsigned int addr, unsigned int size);

#endif
