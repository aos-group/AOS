[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api014.nas"]			; 源程序文件名

	global _api_getkey

[section .text]

_api_getkey:				; int api_getkey(int mode);
	mov edx, 15
	mov eax, [esp + 4]		; mode
	int 0x40
	ret