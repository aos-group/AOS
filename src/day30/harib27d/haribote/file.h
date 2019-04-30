#ifndef __FILE_H
#define __FILE_H

struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};

struct FILEHANDLE {
	char* buf;
	int size;
	int pos;
};

void file_readfat();
void file_loadfile(int clustno, int size, char* buf, char* img);
struct FILEINFO* file_search(char* name, struct FILEINFO* finfo, int max);
char* file_loadfile2(int clustno, int* psize);

#endif
