#include <string.h>
#include "file.h"
#include "memory.h"
#include "asmhead.h"
#include "tek.h"

/**
 * file_readfat - 将磁盘镜像中的 FAT 解压缩
 * @fat: 转换后的 FAT 表
 * @img: 转换前的数据
 */
void file_readfat(int* fat, unsigned char* img) {
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {		/* 2880 个扇区 */
		fat[i + 0] = (img[j + 0] | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
}

/**
 * file_loadfile - 将磁盘 @img 从 @clustno 簇开始的 @size 个字节读到 @buf 缓冲区中
 * @fat: 参考的 fat 表
 */
void file_loadfile(int clustno, int size, char* buf, int* fat, char* img) {
	int i;
	while (1) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = img[clustno * 512 + i];
			}
			return;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
}

char* file_loadfile2(int clustno, int* psize, int* fat) {
	int size = *psize, size2;
	char* buf, *buf2;
	buf = (unsigned char*)memman_alloc_4k(memman, size);
	file_loadfile(clustno, size, buf, fat, (char*)(ADR_DISKIMG + 0x003e00));
	if (size >= 17) {
		/* tek 格式的文件必须带有一个用于识别格式的文件头，这个文件头的部分至少有 17 字节，因此只对大于 17 字节的文件判断其是否为 tek 格式 */
		size2 = tek_getsize(buf);
		if (size2 > 0) {	/* 使用 tek 格式压缩的文件 */
			buf2 = (char*)memman_alloc_4k(memman, size2);
			tek_decomp(buf, buf2, size2);
			memman_free_4k(memman, (int)buf, size);
			buf = buf2;
			*psize = size2;
		}
	}
	return buf;
}

struct FILEINFO* file_search(char* name, struct FILEINFO* finfo, int max) {
	int x, y;
	char s[13];
	for (y = 0; y < 12; y++) {
		s[y] = ' ';
	}
	s[12] = 0;	/* finfo[x].name 长度为 12 字节 */
	for (x = 0, y = 0; y < 11 && name[x]; x++) {
		if (name[x] == '.' && y <= 8) {
			y = 8;
		} else {
			s[y] = name[x];
			if ('a' <= s[y] && s[y] <= 'z') {
				s[y] -= 0x20;	/* 将小写字母转换成大写字母 */
			}
			y++;
		}
	}
	/* 寻找文件 */
	for (x = 0; x < max; x++) {
		if (finfo[x].name[0] == 0x00) {
			return 0;
		}
		if ((finfo[x].type & 0x18) == 0 && strcmp(finfo[x].name, s) == 0) {
			return finfo + x;
		}
	}
	return 0;
}
