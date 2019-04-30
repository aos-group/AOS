void io_hlt(void);

void HariMain(void) {
fin:
	io_hlt();
	goto fin;
}				// 需要由 '\n' 结尾，否则会有警告
