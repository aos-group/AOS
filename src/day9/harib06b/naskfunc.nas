[format "WCOFF"]			; 制作目标文件的模式
[instrset "i486p"]
[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息
[file "naskfunc.nas"]		; 源程序文件名

	global _io_hlt, _io_cli, _io_sti, _io_stihlt
	global _io_in8, _io_in16, _io_in32
	global _io_out8, _io_out16, _io_out32
	global _io_load_eflags, _io_store_eflags
	global _load_gdtr, _load_idtr
	global _asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
	global _load_cr0, _store_cr0
	extern _inthandler21, _inthandler27, _inthandler2c

; 以下是实际的函数
[section .text]
_io_hlt:					; void io_hlt(void);
	hlt
	ret

_io_cli:					; void io_cli(void);
	cli
	ret

_io_sti:					; void io_sti(void);
	sti
	ret

_io_stihlt:					; void io_stihlt(void);
	sti
	hlt
	ret

_io_in8:					; int io_in8(int port);
	mov edx, [esp + 4]
	mov eax, 0
	in al, dx
	ret

_io_in16:					; int io_in16(int port);
	mov edx, [esp + 4]
	mov eax, 0
	in ax, dx
	ret

_io_in32:					; int io_in32(int port);
	mov edx, [esp + 4]
	in eax, dx
	ret

_io_out8:					; void io_out8(int port, int data);
	mov edx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

_io_out16:					; void io_out16(int port, int data);
	mov edx, [esp + 4]
	mov ax, [esp + 8]
	out dx, ax
	ret

_io_out32:					; void io_out32(int port, int data);
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

_io_load_eflags:			; int io_load_eflags(void);
	pushfd					; push flags double-word
	pop eax
	ret

_io_store_eflags:			; void io_store_eflags(int eflags);
	mov eax, [esp + 4]
	push eax
	popfd
	ret

_load_gdtr:					; void load_gdtr(int limit, int addr);
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lgdt [esp + 6]
	ret

_load_idtr:					; void load_idtr(int limit, int addr);
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lidt [esp + 6]
	ret

_asm_inthandler21:
	push es
	push ds
	pushad
	mov eax, esp
	push eax				; 函数的参数
	mov ax, ss 				; c 语言认为 ss, ds, es 指向同一个段
	mov ds, ax
	mov es, ax
	call _inthandler21
	pop eax
	popad
	pop ds
	pop es
	iretd

_asm_inthandler27:
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler27
	pop eax
	popad
	pop ds
	pop es
	iretd

_asm_inthandler2c:
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler2c
	pop eax
	popad
	pop ds
	pop es
	iretd

_load_cr0:						; int load_cr0(void);
	mov eax, cr0
	ret

_store_cr0:						; void store_cr0(int cr0);
	mov eax, [esp + 4]
	mov cr0, eax
	ret
