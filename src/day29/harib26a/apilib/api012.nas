[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api012.nas"]			; 源程序文件名

	global _api_refreshwin

[section .text]

_api_refreshwin:			; void api_refreshwin(int win, int x0, int y0, int x1, int y1);
	push edi
	push esi
	push ebx
	mov edx, 12
	mov ebx, [esp + 16]		; win
	mov eax, [esp + 20]		; x0
	mov ecx, [esp + 24]		; y0
	mov esi, [esp + 28]		; x1
	mov edi, [esp + 32]		; y1
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret