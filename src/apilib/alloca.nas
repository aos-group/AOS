[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global _alloca

[section .text]

_alloca:
	add eax, -4
	sub esp, eax
	jmp dword [esp + eax]	; 代替 ret
