[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api006.nas"]			; 源程序文件名

	global _api_putstrwin

[section .text]

_api_putstrwin:				; void api_putstrwin(int win, int x, int y, int col, int len, char* str);
	push edi
	push esi
	push ebp
	push ebx
	mov edx, 6
	mov ebx, [esp + 20]		; win
	mov esi, [esp + 24]		; x
	mov edi, [esp + 28]		; y
	mov eax, [esp + 32]		; col
	mov ecx, [esp + 36]		; len
	mov ebp, [esp + 40]		; str
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret