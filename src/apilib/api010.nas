[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_free

[section .text]

api_free:					; void api_free(char* addr, int size);
	push ebx
	mov edx, 10
	mov ebx, [cs:0x0020]
	mov eax, [esp + 8]		; addr
	mov ecx, [esp + 12]		; size
	pop ebx
	ret
