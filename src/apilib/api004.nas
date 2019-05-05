[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_end

[section .text]

api_end:					; void api_end(void);
	mov edx, 4
	int 0x40
