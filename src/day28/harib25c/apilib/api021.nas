[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api021.nas"]			; 源程序文件名

	global _api_fclose

[section .text]

_api_fclose:				; void api_fclose(int fhandle);
	mov edx, 22
	mov eax, [esp + 4]		; fhandle
	int 0x40
	ret