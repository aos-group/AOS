[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api027.nas"]			; 源程序文件名

	global _api_getlang

[section .text]

_api_getlang:				; int api_getlang(void);
	mov edx, 27
	int 0x40
	ret