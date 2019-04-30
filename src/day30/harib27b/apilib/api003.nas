[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api003.nas"]			; 源程序文件名

	global _api_putstr1

[section .text]

_api_putstr1:				; void api_putstr1(char* s, int l);
	push ebx
	mov edx, 3
	mov ebx, [esp + 8]		; s
	mov ecx, [esp + 12]		; l
	int 0x40
	pop ebx
	ret