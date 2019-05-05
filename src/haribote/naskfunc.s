[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	global io_hlt, io_cli, io_sti, io_stihlt
	global io_in8, io_in16, io_in32
	global io_out8, io_out16, io_out32
	global io_load_eflags, io_store_eflags
	global load_gdtr, load_idtr
	global asm_inthandler20, asm_inthandler21, asm_inthandler27, asm_inthandler2c, asm_inthandler0d, asm_inthandler0c
	global load_cr0, store_cr0
	global memtest_sub
	global load_tr
	global farjmp, farcall
	global asm_hrb_api, start_app, asm_end_app
	extern inthandler20, inthandler21, inthandler27, inthandler2c, inthandler0d, inthandler0c
	extern hrb_api

; 以下是实际的函数
[section .text]
io_hlt:					; void io_hlt(void);
	hlt
	ret

io_cli:					; void io_cli(void);
	cli
	ret

io_sti:					; void io_sti(void);
	sti
	ret

io_stihlt:					; void io_stihlt(void);
	sti
	hlt
	ret

io_in8:					; int io_in8(int port);
	mov edx, [esp + 4]
	mov eax, 0
	in al, dx
	ret

io_in16:					; int io_in16(int port);
	mov edx, [esp + 4]
	mov eax, 0
	in ax, dx
	ret

io_in32:					; int io_in32(int port);
	mov edx, [esp + 4]
	in eax, dx
	ret

io_out8:					; void io_out8(int port, int data);
	mov edx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

io_out16:					; void io_out16(int port, int data);
	mov edx, [esp + 4]
	mov ax, [esp + 8]
	out dx, ax
	ret

io_out32:					; void io_out32(int port, int data);
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

io_load_eflags:			; int io_load_eflags(void);
	pushfd					; push flags double-word
	pop eax
	ret

io_store_eflags:			; void io_store_eflags(int eflags);
	mov eax, [esp + 4]
	push eax
	popfd
	ret

load_gdtr:					; void load_gdtr(int limit, int addr);
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lgdt [esp + 6]
	ret

load_idtr:					; void load_idtr(int limit, int addr);
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lidt [esp + 6]
	ret

asm_inthandler0c:
	sti
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss 				; c 语言认为 ss, ds, es 指向同一个段
	mov ds, ax
	mov es, ax
	call inthandler0c
	cmp eax, 0
	jne asm_end_app
	pop eax
	popad
	pop ds
	pop es
	add esp, 4				; int 0x0d 会压入一个错误码
	iretd

asm_inthandler0d:
	sti
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	call inthandler0d
	cmp eax, 0
	jne asm_end_app
	pop eax
	popad
	pop ds
	pop es
	add esp, 4				; int 0x0d 会压入一个错误码
	iretd

asm_inthandler20:
	push es
	push ds
	pushad
	mov eax, esp
	push eax				; 函数的参数
	mov ax, ss
	mov ds, ax
	mov es, ax
	call inthandler20
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_inthandler21:
	push es
	push ds
	pushad
	mov eax, esp
	push eax				; 函数的参数
	mov ax, ss
	mov ds, ax
	mov es, ax
	call inthandler21
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_inthandler27:
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	call inthandler27
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_inthandler2c:
	push es
	push ds
	pushad
	mov eax, esp
	push eax
	mov ax, ss
	mov ds, ax
	mov es, ax
	call inthandler2c
	pop eax
	popad
	pop ds
	pop es
	iretd

load_cr0:						; int load_cr0(void);
	mov eax, cr0
	ret

store_cr0:						; void store_cr0(int cr0);
	mov eax, [esp + 4]
	mov cr0, eax
	ret

load_tr:						; void load_tr(int tr);
	ltr [esp + 4]
	ret

farjmp:						; void farjmp(int eip, int cs);
	jmp far [esp + 4]			; eip, cs
	ret

farcall:						; void farcall(int eip, int cs);
	call far [esp + 4]			; eip, cs
	ret

memtest_sub:					; unsigned int memtest_sub(unsigned int start, unsigned int end)
	push edi
	push esi
	push ebx
	mov esi, 0xaa55aa55
	mov edi, 0x55aa55aa
	mov eax, [esp + 12 + 4]		; start
mts_loop:
	mov ebx, eax
	add ebx, 0xffc				; 4KB 里的最后 4 个字节
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

asm_hrb_api:
	sti
	push ds
	push es
	pushad						; 用于保存寄存器值的 push
	pushad						; 用于向 hrb_api 传值的 push
	mov ax, ss
	mov ds, ax					; 将操作系统用段地址存入 ds 和 es
	mov es, ax
	call hrb_api
	cmp eax, 0
	jne asm_end_app
	add esp, 32
	popad
	pop es
	pop ds
	iretd
asm_end_app: 					; eax 为 tss.esp0 的地址
	mov esp, [eax]
	mov dword [eax + 4], 0		; tss.ss0
	popad
	ret 						; 返回 cmd_app

start_app:						; void start_app(int eip, int cs, int esp, int ds, int* tss_esp0);
	pushad
	mov eax, [esp + 36]			; 应用程序用 eip
	mov ecx, [esp + 40]			; 应用程序用 cs
	mov edx, [esp + 44]			; 应用程序用 esp
	mov ebx, [esp + 48]			; 应用程序用 ds/ss
	mov ebp, [esp + 52]			; tss.esp0 的地址
	mov [ebp], esp 				; 保存操作系统用 esp
	mov [ebp + 4], ss 			; 保存操作系统用 ss
	mov es, bx
	mov ds, bx
	mov fs, bx
	mov gs, bx
; 下面调整栈，以免用 retf 跳转到应用程序
	or ecx, 3					; 应用程序段选择子
	or ebx, 3					; 应用程序段选择子
	push ebx					; 应用程序用 ss
	push edx					; 应用程序用 esp
	push ecx					; 应用程序用 cs
	push eax					; 应用程序用 eip
	retf
; 应用程序结束后不会回到这里
