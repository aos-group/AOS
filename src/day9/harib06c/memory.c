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
