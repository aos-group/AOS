[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api007.nas"]			; 源程序文件名

	global _api_initmalloc

[section .text]

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