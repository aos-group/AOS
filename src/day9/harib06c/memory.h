#ifndef __MEMORY_H
#define __MEMORY_H

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end);

/**
 * memtest_sub - 调查从 start 地址到 end 地址的范围内，能够使用的内存的末尾地址
 * @start: 内存起始地址
 * @end: 内存结束地址
 */
unsigned int memtest_sub(unsigned int start, unsigned int end);

#endif
