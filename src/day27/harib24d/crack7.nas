[format "WCOFF"]
[instrset "i486p"]
[bits 32]
[file "crack7.nas"]

	global _HariMain

[section .text]

_HariMain:
	mov ax, 4
	mov ds, ax
	cmp dword [ds:0x0004], 'Hari'
	jne fin					; 不是应用程序，因此不执行任何操作
	mov ecx, [ds:0x0000]	; 读取该应用程序数据段的大小
	mov ax, 12
	mov ds, ax

crackloop:					; 整个用 123 填充
	add ecx, -1
	mov byte [ds:ecx], 123
	cmp ecx, 0
	jne crackloop

fin:						; 结束
	mov edx, 4
	int 0x40