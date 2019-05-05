[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_fsize

[section .text]

api_fsize:					; int api_fsize(int fhandle, int mode);
	mov edx, 24
	mov eax, [esp + 4]		; fhandle
	mov ecx, [esp + 8]		; mode
	int 0x40
	ret
