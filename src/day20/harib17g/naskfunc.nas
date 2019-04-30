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
	global _asm_inthandler20, _asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
	global _load_cr0, _store_cr0
	global _memtest_sub
	global _load_tr
	global _farjmp, _farcall
	global _asm_cons_putchar
	extern _inthandler20, _inthandler21, _inthandler27, _inthandler2c
	extern _cons_putchar

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

_asm_inthandler20:
	push es
	push ds
	pushad
	mov eax, esp
	push eax				; 函数的参数
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler20
	pop eax
	popad
	pop ds
	pop es
	iretd

_asm_inthandler21:
	push es
	push ds
	pushad
	mov eax, esp
	push eax				; 函数的参数
	mov ax, ss
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

_load_tr:						; void load_tr(int tr);
	ltr [esp + 4]
	ret

_farjmp:						; void farjmp(int eip, int cs);
	jmp far [esp + 4]			; eip, cs
	ret

_farcall:						; void farcall(int eip, int cs);
	call far [esp + 4]			; eip, cs
	ret

_memtest_sub:					; unsigned int memtest_sub(unsigned int start, unsigned int end)
	push edi
	push esi
	push ebx
	mov esi, 0xaa55aa55
	mov edi, 0x55aa55aa
	mov eax, [esp + 12 + 4]		; start
mts_loop:
	mov ebx, eax
	add ebx, 0xffc
	mov edx, [ebx]				; old
	mov [ebx], esi
	xor dword [ebx], 0xffffffff
	cmp edi, [ebx]
	jne mts_fin
	xor dword [ebx], 0xffffffff
	cmp esi, [ebx]
	jne mts_fin
	mov [ebx], edx
	add eax, 0x1000
	cmp eax, [esp + 12 + 8]		; end
	jbe mts_loop
	jmp fin
mts_fin:
	mov [ebx], edx
fin:
	pop ebx
	pop esi
	pop edi
	ret

_asm_cons_putchar:
	sti
	pushad
	push 1
	and eax, 0xff 					; 将 ah 和 eax 的高位置 1, 将 eax 置为已存入字符编码的状态
	push eax
	push dword [0x0fec]
	call _cons_putchar
	add esp, 12
	popad
	iretd
