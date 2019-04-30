[format "WCOFF"]
[instrset "i486p"]
[bits 32]
[file "hello2.nas"]

	global _HariMain

[section .text]

_HariMain:
	mov edx, 2
	mov ebx, msg
	int 0x40
	mov edx, 4
	int 0x40

[section .data]

msg:
	db "hello, world", 0x0a, 0