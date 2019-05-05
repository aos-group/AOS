[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global api_fclose

[section .text]

api_fclose:				; void api_fclose(int fhandle);
	mov edx, 22
	mov eax, [esp + 4]		; fhandle
	int 0x40
	ret
