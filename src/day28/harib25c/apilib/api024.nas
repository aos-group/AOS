[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api024.nas"]			; 源程序文件名

	global _api_fread

[section .text]

_api_fread:					; int api_fread(char* buf, int maxsize, int fhandle);
	push ebx
	mov edx, 25
	mov eax, [esp + 16]		; fhandle
	mov ecx, [esp + 12]		; maxsize
	mov ebx, [esp + 8]		; buf
	int 0x40
	pop ebx
	ret