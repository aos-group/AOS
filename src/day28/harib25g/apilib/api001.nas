[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api001.nas"]			; 源程序文件名

	global _api_putchar

[section .text]

_api_putchar:				; void api_putchar(int c);
	mov edx, 1
	mov al, [esp + 4]
	int 0x40
	ret