[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_putstr0

[section .text]

api_putstr0:				; void api_putstr0(char* s);
	push ebx
	mov edx, 2
	mov ebx, [esp + 8]
	int 0x40
	pop ebx
	ret
