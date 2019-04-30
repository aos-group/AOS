[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "a_nask.nas"]			; 源程序文件名

	global _api_putchar
	global _api_end
	global _api_putstr0

[section .text]

_api_putchar:				; void api_putchar(int c);
	mov edx, 1
	mov al, [esp + 4]
	int 0x40
	ret

_api_end:					; void api_end(void);
	mov edx, 4
	int 0x40

_api_putstr0:				; void api_putstr0(char* s);
	push ebx
	mov edx, 2
	mov ebx, [esp + 8]
	int 0x40
	pop ebx
	ret