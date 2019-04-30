[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api020.nas"]			; 源程序文件名

	global _api_fopen

[section .text]

_api_fopen:					; int api_fopen(char* fname);
	push ebx
	mov edx, 21
	mov ebx, [esp + 8]		; fname
	int 0x40
	pop ebx
	ret