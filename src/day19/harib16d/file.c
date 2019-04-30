#include "file.h"

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
