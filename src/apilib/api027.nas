[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_getlang

[section .text]

api_getlang:				; int api_getlang(void);
	mov edx, 27
	int 0x40
	ret
