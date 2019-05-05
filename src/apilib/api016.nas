[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_alloctimer

[section .text]

api_alloctimer:			; int api_alloctimer(void);
	mov edx, 16
	int 0x40
	ret
