[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_putchar

[section .text]

api_putchar:				; void api_putchar(int c);
	mov edx, 1
	mov al, [esp + 4]
	int 0x40
	ret
