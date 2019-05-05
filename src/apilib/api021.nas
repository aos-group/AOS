[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_fopen

[section .text]

api_fopen:					; int api_fopen(char* fname);
	push ebx
	mov edx, 21
	mov ebx, [esp + 8]		; fname
	int 0x40
	pop ebx
	ret
