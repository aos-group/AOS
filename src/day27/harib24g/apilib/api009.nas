[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api009.nas"]			; 源程序文件名

	global _api_free

[section .text]

_api_free:					; void api_free(char* addr, int size);
	push ebx
	mov edx, 10
	mov ebx, [cs:0x0020]
	mov eax, [esp + 8]		; addr
	mov ecx, [esp + 12]		; size
	pop ebx
	ret