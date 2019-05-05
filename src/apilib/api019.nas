[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_freetimer

[section .text]

api_freetimer:				; void api_freetimer(int timer);
	push ebx
	mov edx, 19
	mov ebx, [esp + 8]		; timer
	int 0x40
	pop ebx
	ret
