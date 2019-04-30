#include "memory.h"
#include "naskfunc.h"

struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;

/**
 * memtest - 检查内存容量
 * @start: 内存起始地址
 * @end: 内存结束地址
 */
unsigned int memtest(unsigned int start, unsigned int end) {
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 确认 CPU 是 386 还是 486 以上的 */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0)	/* 如果是 386, 即使设定 AC = 1, AC 的值还会自动回到 0 */
		flg486 = 1;
	eflg &= ~EFLAGS_AC_BIT;
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;		/* 禁止缓存 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;		/* 允许缓存 */
		store_cr0(cr0);	
	}

	return i;
}

void memman_init(struct MEMMAN* man) {
	int i;
	man->frees = 0;		/* 可用信息数目 */
	man->maxfrees = 0;	/* 用于观察可用状况：frees 的最大值 */
	man->lostsize = 0;	/* 释放失败的内存大小总和 */
	man->lostsize = 0;	/* 释放失败次数 */
	man->free_link = -1;
	man->unused_link = 0;
	for (i = 0; i < MEMMAN_FREES - 1; i++)
		man->free[i].next = i + 1;
	man->free[i].next = -1;
}

unsigned int memman_total(struct MEMMAN* man) {
	unsigned int t = 0, i;
	for (i = man->free_link; i != -1; i = man->free[i].next)
		t += man->free[i].size;
	return t;
}

/**
 * memman_remove - 将下标为 cur 的元素放到 unused_link 中
 * @man: 内存管理器
 * @prev: 前一个下标
 * @cur: 当前下标
 */
void memman_remove(struct MEMMAN* man, int prev, int cur) {
	man->frees--;
	if (prev != -1) {
		man->free[prev].next = man->free[cur].next;
	} else {
		man->free_link = man->free[cur].next;
	}
	man->free[cur].next = man->unused_link;
	man->unused_link = cur;
}

/**
 * memman_add - 从 unused_link 中选择一个元素添加到 free_link 中
 * @man: 内存管理器
 * @prev: 前一个下标
 * @cur: 下一个下标
 * @return: 选择的元素下标
 */
int memman_add(struct MEMMAN* man, int prev, int next) {
	int cur = man->unused_link;
	man->unused_link = man->free[cur].next;
	man->free[cur].next = next;
	if (prev != -1) {
		man->free[prev].next = cur;
	} else {
		man->free_link = cur;
	}
	man->frees++;
	man->maxfrees = man->maxfrees > man->frees ? man->maxfrees : man->frees;
	return cur;
}

unsigned int memman_alloc(struct MEMMAN* man, unsigned int size) {
	unsigned int i, a, prev = -1;
	for (i = man->free_link; i != -1; prev = i, i = man->free[i].next) {
		if (man->free[i].size >= size) {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				memman_remove(man, prev, i);
			}
			return a;
		}
	}
	return 0;
}

int memman_free(struct MEMMAN* man, unsigned int addr, unsigned int size) {
	int i, prev = -1;
	for (i = man->free_link; i != -1; prev = i, i = man->free[i].next) {
		if (man->free[i].addr > addr)
			break;
	}
	if (prev != -1 && man->free[prev].addr + man->free[prev].size == addr) {
		/* 可以与前面的内存归纳到一起 */
		man->free[prev].size += size;
		if (i != -1 && addr + size == man->free[i].addr) {
			/* 可以与后面的内存归纳到一起 */
			man->free[prev].size += man->free[i].size;
			memman_remove(man, prev, i);
		}
		return 0;
	}
	/* 不能与前面的内存归纳到一起 */
	if (i != -1 && addr + size == man->free[i].addr) {
		man->free[i].addr = addr;
		man->free[i].size += size;
		return 0;
	}
	/* 既不能与前面归纳到一起，也不能与后面归纳到一起 */
	if (man->unused_link != -1) {
		i = memman_add(man, prev, i);
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	/* 不能往后移动 */
	man->losts++;
	man->lostsize += size;
	return -1;
}

/**
 * memman_alloc_4k - 分配 4KB 的整数倍的内存
 */
unsigned int memman_alloc_4k(struct MEMMAN* man, unsigned int size) {
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

/**
 * memman_free_4k - 释放 4KB 的整数倍的内存
 */
int memman_free_4k(struct MEMMAN* man, unsigned int addr, unsigned int size) {
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
