#include "memory.h"
#include "naskfunc.h"

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

/**
 * memtest_sub - 调查从 start 地址到 end 地址的范围内，能够使用的内存的末尾地址
 * @start: 内存起始地址
 * @end: 内存结束地址
 */
unsigned int memtest_sub(unsigned int start, unsigned int end) {
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for (i = start; i <= end; i += 0x1000) {	/* 每次检查 4KB */
		p = (unsigned int*)(i + 0xffc);		/* 只检查最后 4 字节 */
		old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if (*p != pat1) {	/* 检查反转结果是否正确 */
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;
		if (*p != pat0)		/* 检查值是否恢复 */
			goto not_memory;
		*p = old;
	}
	return i;
}
