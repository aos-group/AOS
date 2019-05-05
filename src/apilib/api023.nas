[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_fseek

[section .text]

api_fseek:					; void api_fseek(int fhandle, int offset, int mode);
	push ebx
	mov edx, 23
	mov eax, [esp + 8]		; fhandle
	mov ecx, [esp + 16]		; mode
	mov ebx, [esp + 12]		; offset
	int 0x40
	pop ebx
	ret
