[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "api00.2nas"]			; 源程序文件名

	global _api_end

[section .text]

_api_end:					; void api_end(void);
	mov edx, 4
	int 0x40