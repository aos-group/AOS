#include "memory.h"
#include "naskfunc.h"

struct MEMMAN* memman = (struct MEMMAN*)MEMMAN_ADDR;
int memtotal = 0;

/**
 * memtest - 检查内存容量
 * @start: 内存起始地址
 * @end: 内存结束地址
 */
void memtest(unsigned int start, unsigned int end) {
	char flg486 = 0;
	unsigned int eflg, cr0;

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

	memtotal = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;		/* 允许缓存 */
		store_cr0(cr0);	
	}
}

void memman_init(struct MEMMAN* man) {
	man->frees = 0;		/* 可用信息数目 */
	man->maxfrees = 0;	/* 用于观察可用状况：frees 的最大值 */
	man->lostsize = 0;	/* 释放失败的内存大小总和 */
	man->losts = 0;		/* 释放失败次数 */
}

unsigned int memman_total(struct MEMMAN* man) {
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++)
		t += man->free[i].size;
	return t;
}

unsigned int memman_alloc(struct MEMMAN* man, unsigned int size) {
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				man->frees--;
				for (; i < man->frees; i++)
					man->free[i] = man->free[i + 1];
			}
			return a;
		}
	}
	return 0;
}

int memman_free(struct MEMMAN* man, unsigned int addr, unsigned int size) {
	int i, j;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr)
			break;
	}
	if (i > 0 && man->free[i - 1].addr + man->free[i - 1].size == addr) {
		/* 可以与前面的内存归纳到一起 */
		man->free[i - 1].size += size;
		if (i < man->frees && addr + size == man->free[i].addr) {
			/* 可以与后面的内存归纳到一起 */
			man->free[i - 1].size += man->free[i].size;
			man->frees--;
			for (; i < man->frees; i++)
				man->free[i] = man->free[i + 1];
		}
		return 0;
	}
	/* 不能与前面的内存归纳到一起 */
	if (i < man->frees && addr + size == man->free[i].addr) {
		man->free[i].addr = addr;
		man->free[i].size += size;
		return 0;
	}
	/* 既不能与前面归纳到一起，也不能与后面归纳到一起 */
	if (man->frees < MEMMAN_FREES) {
		for (j = man->frees; j > i; j--)
			man->free[j] = man->free[j - 1];
		man->frees++;
		if (man->maxfrees < man->frees)
			man->maxfrees = man->frees;
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
