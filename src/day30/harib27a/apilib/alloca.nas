[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "alloca.nas"]			; 源程序文件名

	global __alloca

[section .text]

__alloca:
	add eax, -4
	sub esp, eax
	jmp dword [esp + eax]	; 代替 ret