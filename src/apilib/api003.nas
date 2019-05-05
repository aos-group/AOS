[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_putstr1

[section .text]

api_putstr1:				; void api_putstr1(char* s, int l);
	push ebx
	mov edx, 3
	mov ebx, [esp + 8]		; s
	mov ecx, [esp + 12]		; l
	int 0x40
	pop ebx
	ret
