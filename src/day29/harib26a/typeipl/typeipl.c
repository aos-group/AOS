#include <stdio.h>
#include "apilib.h"

void HariMain(void) {
	int fh;
	char c;
	fh = api_fopen("ipl10.nas");
	if (fh) {
		while (1) {
			if (api_fread(&c, 1, fh) == 0) {
				break;
			}
			api_putchar(c);
		}
	}
	api_end();
}
