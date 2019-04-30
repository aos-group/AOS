void io_hlt(void);
void write_mem8(int addr, int data);

void HariMain(void) {
	int i;

	for (i = 0xa0000; i <= 0xaffff; i++)
		write_mem8(i, 15);

	while (1)
		io_hlt();
}				// 需要由 '\n' 结尾，否则会有警告
