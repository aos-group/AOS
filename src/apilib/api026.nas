[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_cmdline

[section .text]

api_cmdline:				; int api_cmdline(char* buf, int maxsize);
	push ebx
	mov edx, 26
	mov ecx, [esp + 12]		; maxsize
	mov ebx, [esp + 8]		; buf
	int 0x40
	pop ebx
	ret
