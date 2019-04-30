[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "a_nask.nas"]			; 源程序文件名

	global _api_putchar
	global _api_end
	global _api_putstr0
	global _api_openwin
	global _api_putstrwin
	global _api_boxfilwin

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

_api_boxfilwin:				; void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
	push edi
	push esi
	push ebp
	push ebx
	mov edx, 7
	mov ebx, [esp + 20]		; win
	mov eax, [esp + 24]		; x0
	mov ecx, [esp + 28]		; y0
	mov esi, [esp + 32]		; x1
	mov edi, [esp + 36]		; y1
	mov ebp, [esp + 40]		; col
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret