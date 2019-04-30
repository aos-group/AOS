[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api018.nas"]			; 源程序文件名

	global _api_settimer

[section .text]

_api_settimer:				; void api_settimer(int timer, int time);
	push ebx
	mov edx, 18
	mov ebx, [esp + 8]		; timer
	mov eax, [esp + 12]		; time
	int 0x40
	pop ebx
	ret