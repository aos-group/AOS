[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api013.nas"]			; 源程序文件名

	global _api_closewin

[section .text]

_api_closewin:				; void api_closewin(int win);
	push ebx
	mov edx, 14
	mov ebx, [esp + 8]		; win
	int 0x40
	pop ebx
	ret