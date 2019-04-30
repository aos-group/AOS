[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api026.nas"]			; 源程序文件名

	global _api_cmdline

[section .text]

_api_cmdline:				; int api_cmdline(char* buf, int maxsize);
	push ebx
	mov edx, 26
	mov ecx, [esp + 12]		; maxsize
	mov ebx, [esp + 8]		; buf
	int 0x40
	pop ebx
	ret