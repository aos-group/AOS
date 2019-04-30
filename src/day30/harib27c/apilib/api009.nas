[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api009.nas"]			; 源程序文件名

	global _api_malloc

[section .text]

_api_malloc:				; char* api_malloc(int size);
	push ebx
	mov edx, 9
	mov ebx, [cs:0x0020]
	mov ecx, [esp + 8]		; size
	pop ebx
	ret