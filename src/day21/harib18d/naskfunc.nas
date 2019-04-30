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
	global _asm_hrb_api, _start_app
	extern _inthandler20, _inthandler21, _inthandler27, _inthandler2c
	extern _hrb_api

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
	mov ax, ss
	cmp ax, 1 * 8
	jne .from_app
	mov eax, esp
	push ss 				; 保存中断时的 ss
	push eax				; 保存中断时的 esp
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler20
	add esp, 8
	jmp .fin

.from_app:					; 当应用程序活动时发生中断
	mov eax, 1 * 8
	mov ds, ax
	mov ecx, [0xfe4]		; 操作系统的 esp
	add ecx, -8
	mov [ecx + 4], ss 		; 保存中断时的 ss
	mov [ecx], esp 			; 保存中断时的 esp
	mov ss, ax
	mov es, ax
	mov esp, ecx 			; 切换到内核栈
	call _inthandler20
	pop ecx
	pop eax
	mov ss, ax				; 将 ss 设回应用程序用
	mov esp, ecx			; 将 esp 设回应用程序用
.fin:
	popad
	pop ds
	pop es
	iretd

_asm_inthandler21:
	push es
	push ds
	pushad
	mov ax, ss
	cmp ax, 1 * 8
	jne .from_app
	mov eax, esp
	push ss 				; 保存中断时的 ss
	push eax				; 保存中断时的 esp
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler21
	add esp, 8
	jmp .fin

.from_app:					; 当应用程序活动时发生中断
	mov eax, 1 * 8
	mov ds, ax
	mov ecx, [0xfe4]		; 操作系统的 esp
	add ecx, -8
	mov [ecx + 4], ss 		; 保存中断时的 ss
	mov [ecx], esp 			; 保存中断时的 esp
	mov ss, ax
	mov es, ax
	mov esp, ecx 			; 切换到内核栈
	call _inthandler21
	pop ecx
	pop eax
	mov ss, ax				; 将 ss 设回应用程序用
	mov esp, ecx			; 将 esp 设回应用程序用
.fin:
	popad
	pop ds
	pop es
	iretd

_asm_inthandler27:
	push es
	push ds
	pushad
	mov ax, ss
	cmp ax, 1 * 8
	jne .from_app
	mov eax, esp
	push ss 				; 保存中断时的 ss
	push eax				; 保存中断时的 esp
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler27
	add esp, 8
	jmp .fin

.from_app:					; 当应用程序活动时发生中断
	mov eax, 1 * 8
	mov ds, ax
	mov ecx, [0xfe4]		; 操作系统的 esp
	add ecx, -8
	mov [ecx + 4], ss 		; 保存中断时的 ss
	mov [ecx], esp 			; 保存中断时的 esp
	mov ss, ax
	mov es, ax
	mov esp, ecx 			; 切换到内核栈
	call _inthandler27
	pop ecx
	pop eax
	mov ss, ax				; 将 ss 设回应用程序用
	mov esp, ecx			; 将 esp 设回应用程序用
.fin:
	popad
	pop ds
	pop es
	iretd

_asm_inthandler2c:
	push es
	push ds
	pushad
	mov ax, ss
	cmp ax, 1 * 8
	jne .from_app
	mov eax, esp
	push ss 				; 保存中断时的 ss
	push eax				; 保存中断时的 esp
	mov ax, ss
	mov ds, ax
	mov es, ax
	call _inthandler2c
	add esp, 8
	jmp .fin

.from_app:					; 当应用程序活动时发生中断
	mov eax, 1 * 8
	mov ds, ax
	mov ecx, [0xfe4]		; 操作系统的 esp
	add ecx, -8
	mov [ecx + 4], ss 		; 保存中断时的 ss
	mov [ecx], esp 			; 保存中断时的 esp
	mov ss, ax
	mov es, ax
	mov esp, ecx 			; 切换到内核栈
	call _inthandler2c
	pop ecx
	pop eax
	mov ss, ax				; 将 ss 设回应用程序用
	mov esp, ecx			; 将 esp 设回应用程序用
.fin:
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

_asm_hrb_api:
	; 为方便起见从开头就禁止中断请求
	push ds
	push es
	pushad						; 用于保存寄存器值的 push
	mov eax, 1 * 8
	mov ds, ax
	mov ecx, [0xfe4]			; 操作系统的 esp
	add ecx, -40
	mov [ecx + 32], esp			; 保存应用程序的 esp
	mov [ecx + 36], ss 			; 保存应用程序的 ss

; 将 pushad 后的值复制到系统栈
	mov edx, [esp]
	mov ebx, [esp + 4]
	mov [ecx], edx
	mov [ecx + 4], ebx
	mov edx, [esp + 8]
	mov ebx, [esp + 12]
	mov [ecx + 8], edx
	mov [ecx + 12], ebx
	mov edx, [esp + 16]
	mov ebx, [esp + 20]
	mov [ecx + 16], edx
	mov [ecx + 20], ebx
	mov edx, [esp + 24]
	mov ebx, [esp + 28]
	mov [ecx + 24], edx
	mov [ecx + 28], ebx

	mov es, ax
	mov ss, ax
	mov esp, ecx				; 切换到内核栈
	sti 						; 恢复中断请求

	call _hrb_api

	mov ecx, [esp + 32]			; 取出应用程序的 esp
	mov eax, [esp + 36]			; 取出应用程序的 ss
	cli
	mov ss, ax
	mov esp, ecx				; 切换到用户栈
	popad
	pop es
	pop ds
	iretd						; 这个命令会自动执行 sti

_start_app:						; void start_app(int eip, int cs, int esp, int ds);
	pushad
	mov eax, [esp + 36]			; 应用程序用 eip
	mov ecx, [esp + 40]			; 应用程序用 cs
	mov edx, [esp + 44]			; 应用程序用 esp
	mov ebx, [esp + 48]			; 应用程序用 ds/ss
	mov [0xfe4], esp			; 操作系统用 esp
	cli 						; 在切换过程中禁止中断请求
	mov es, bx
	mov ss, bx
	mov ds, bx
	mov fs, bx
	mov gs, bx
	mov esp, edx				; 切换到用户栈
	sti 						; 切换完成后恢复中断请求
	push ecx					; 用于 far-call 的 push(cs)
	push eax					; 用于 far-call 的 push(eip)
	call far [esp]				; 调用应用程序

; 应用程序结束后返回此处
	mov eax, 1 * 8				; 操作系统用 ds/ss
	cli
	mov es, ax
	mov ss, ax
	mov ds, ax
	mov fs, ax
	mov gs, ax
	mov esp, [0xfe4]			; 切换到内核栈
	sti
	popad
	ret
