[format "WCOFF"]			; 制作目标文件的模式
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "naskfunc.nas"]		; 源文件名信息

	global _io_hlt			; 程序中包含的函数名

; 以下是实际的函数
[section .text]
_io_hlt:					; void io_hlt(void);
	hlt
	ret