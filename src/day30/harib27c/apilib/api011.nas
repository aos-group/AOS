[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api011.nas"]			; 源程序文件名

	global _api_point

[section .text]

_api_point:					; void api_point(int win, int x, int y, int col);
	push edi
	push esi
	push ebx
	mov edx, 11
	mov ebx, [esp + 16]		; win
	mov esi, [esp + 20]		; x
	mov edi, [esp + 24]		; y
	mov eax, [esp + 28]		; col
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret