[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_getkey

[section .text]

api_getkey:				; int api_getkey(int mode);
	mov edx, 15
	mov eax, [esp + 4]		; mode
	int 0x40
	ret
