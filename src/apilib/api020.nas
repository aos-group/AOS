[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_beep

[section .text]

api_beep:					; void api_beep(int tone);
	mov edx, 20
	mov eax, [esp + 4]		; tone
	int 0x40
	ret
