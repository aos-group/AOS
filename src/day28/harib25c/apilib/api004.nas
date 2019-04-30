[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api004.nas"]			; 源程序文件名

	global _api_openwin

[section .text]

_api_openwin:				; int api_openwin(char* buf, int xsize, int ysize, int col_inv, char* title);
	push edi
	push esi
	push ebx
	mov edx, 5
	mov ebx, [esp + 16]		; buf
	mov esi, [esp + 20]		; xsize
	mov edi, [esp + 24]		; ysize
	mov eax, [esp + 28]		; col_inv
	mov ecx, [esp + 32]		; title
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret