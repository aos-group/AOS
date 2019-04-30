[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api023.nas"]			; 源程序文件名

	global _api_fsize

[section .text]

_api_fsize:					; int api_fsize(int fhandle, int mode);
	mov edx, 24
	mov eax, [esp + 4]		; fhandle
	mov ecx, [esp + 8]		; mode
	int 0x40
	ret