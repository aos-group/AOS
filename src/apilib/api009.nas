[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_malloc

[section .text]

api_malloc:				; char* api_malloc(int size);
	push ebx
	mov edx, 9
	mov ebx, [cs:0x0020]
	mov ecx, [esp + 8]		; size
	pop ebx
	ret
