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
	global _api_initmalloc
	global _api_malloc
	global _api_free
	global _api_point
	global _api_refreshwin
	global _api_linewin
	global _api_closewin
	global _api_getkey
	global _api_alloctimer
	global _api_inittimer
	global _api_settimer
	global _api_freetimer

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

_api_initmalloc:			; void api_initmalloc(void);
	push ebx
	mov edx, 8
	mov ebx, [cs:0x0020]	; malloc 内存空间的地址
	mov eax, ebx
	add eax, 32 * 1024		; 32KB, MEMMAN 的大小
	mov ecx, [cs:0x0000]	; 数据段的大小
	sub ecx, eax
	int 0x40
	pop ebx
	ret

_api_malloc:				; char* api_malloc(int size);
	push ebx
	mov edx, 9
	mov ebx, [cs:0x0020]
	mov ecx, [esp + 8]		; size
	pop ebx
	ret

_api_free:					; void api_free(char* addr, int size);
	push ebx
	mov edx, 10
	mov ebx, [cs:0x0020]
	mov eax, [esp + 8]		; addr
	mov ecx, [esp + 12]		; size
	pop ebx
	ret

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

_api_linewin:				; void api_linewin(int win, int x0, int y0, int x1, int y1, int col);
	push edi
	push esi
	push ebp
	push ebx
	mov edx, 13
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

_api_closewin:				; void api_closewin(int win);
	push ebx
	mov edx, 14
	mov ebx, [esp + 8]		; win
	int 0x40
	pop ebx
	ret

_api_getkey:				; int api_getkey(int mode);
	mov edx, 15
	mov eax, [esp + 4]		; mode
	int 0x40
	ret

_api_alloctimer:			; int api_alloctimer(void);
	mov edx, 16
	int 0x40
	ret

_api_inittimer:				; void api_inittimer(int timer, int data);
	push ebx
	mov edx, 17
	mov ebx, [esp + 8]		; timer
	mov eax, [esp + 12]		; data
	int 0x40
	pop ebx
	ret

_api_settimer:				; void api_settimer(int timer, int time);
	push ebx
	mov edx, 18
	mov ebx, [esp + 8]		; timer
	mov eax, [esp + 12]		; time
	int 0x40
	pop ebx
	ret

_api_freetimer:				; void api_freetimer(int timer);
	push ebx
	mov edx, 19
	mov ebx, [esp + 8]		; timer
	int 0x40
	pop ebx
	ret