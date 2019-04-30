[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "naskfunc.nas"]		; 源文件名信息

	global _io_hlt, _write_mem8

; 以下是实际的函数
[section .text]
_io_hlt:					; void io_hlt(void);
	hlt
	ret

_write_mem8:				; void write_mem8(int addr, int data);
	mov ecx, [esp + 4]
	mov al, [esp + 8]
	mov [ecx], al
	ret