[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api017.nas"]			; 源程序文件名

	global _api_inittimer

[section .text]

_api_inittimer:				; void api_inittimer(int timer, int data);
	push ebx
	mov edx, 17
	mov ebx, [esp + 8]		; timer
	mov eax, [esp + 12]		; data
	int 0x40
	pop ebx
	ret