[bits 32]					; 制作 32 位模式用的机器语言

; 制作目标文件的信息

	VMODE	equ		0x0ff2			; 关于颜色数目的信息、颜色的位数
	SCRNX	equ 	0x0ff4			; 分辨率的 X (xcreen x)
	SCRNY	equ		0x0ff6			; 分辨率的 Y (xcreen y)
	VRAM	equ		0x0ff8			; 图像缓冲区的开始地址

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
	global set_mode_0x13
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

;---------------------------------;
;  sets the screen to mode 0x13   ;
;---------------------------------;
set_mode_0x13:
         pushad
         ; push    ax

		 mov byte [VMODE], 8		; 记录画面模式
		 mov word [SCRNX], 320
		 mov word [SCRNY], 200
		 mov dword [VRAM], 0x000a0000

         mov     esi, mode0x13
         call    set_regs

         ; mov     esi, palette256
         ; call    set_palette256

    ;      pop     ax
    ;      cmp     ax, 1
    ;      jne     .cont
    ;      mov     edi, 0xa0000                        ; write directly to mem
    ;      mov     ax, 0x0000                          ; clear all 256kb with color 0
    ;      mov     ecx, 0x20000                        ; 256kb, 0x20000 = (256*1024)/2
    ;      rep     stosw                               ; by wordwrites
    ; .cont:

         popad
         ret

;---------------------------------;
;  Set VGA regs to choosen mode   ;
;           internal use.         ;
;---------------------------------;
set_regs:
         cli
         mov     dx, 0x3C2
         lodsb	; 0x63
         out     dx, al

         mov     dx, 0x3DA
         lodsb	; 0x00
         out     dx, al

         xor     ecx, ecx	; 0
         mov     dx, 0x3C4
    .l1:
         lodsb	; 0x03, 0x01, 0x0F, 0x00, 0x0E
         xchg    al, ah
         mov     al, cl
         out     dx, ax
         inc     ecx
         cmp     cl, 4
         jbe     .l1

         mov     dx, 0x3D4
         mov     ax, 0x0E11
         out     dx, ax

         xor     ecx, ecx	; 0
         mov     dx, 0x3D4
    .l2:	; 设置分辨率
    			;  0	 1	   2	 3	   4	 5	   6	 7	   8	 9	   10	 11	   12	 13	   14	 15	   16	 17    18	 19	   20	 21    22	 23	   24
         lodsb	; 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF
         xchg    al, ah
         mov     al, cl
         out     dx, ax
         inc     ecx
         cmp     cl, 0x18
         jbe     .l2

         xor     ecx, ecx	; 0
         mov     dx, 0x3CE
    .l3:	; 图形地址寄存器
         lodsb	; 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF
         xchg    al, ah
         mov     al, cl
         out     dx, ax
         inc     ecx
         cmp     cl, 8
         jbe     .l3

         mov     dx, 0x3DA
         in      al, dx

         xor     ecx, ecx	; 0
         mov     dx, 0x3C0
    .l4:
         in      ax, dx
         mov     al, cl
         out     dx, al
         lodsb	; 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00
         out     dx, al
         inc     ecx
         cmp     cl, 0x14
         jbe     .l4

         mov     al, 0x20
         out     dx, al

         sti
         ret

;---------------------------------------------;
;  sets the palette (256 colors)              ;
;                                             ;
; input:  esi = palette.                      ;
; output: none.                               ;
;---------------------------------------------;
set_palette256:
         push    ax
         push    cx
         push    dx

         xor     cx, cx
    .l1:
         mov     dx, 0x03C8
         mov     al, cl                              ; color no. = loop no.
         out     dx, al
         inc     dx                                  ; port 0x3C9
         mov     al, byte [esi]                      ; red
         out     dx, al
         inc     esi
         mov     al, byte [esi]                      ; green
         out     dx, al
         inc     esi
         mov     al, byte [esi]                      ; blue
         out     dx, al
         inc     esi

         inc     cx
         cmp     cx, 256
         jl      .l1

         pop     dx
         pop     cx
         pop     ax
         ret

[section .data]
    ;---------------------;
    ;  VGA mode values.   ;
    ;---------------------;

         mode0x13        db   0x63, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x0E, 0x5F, 0x4F
                         db   0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00
                         db   0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E, 0x8F, 0x28
                         db   0x40, 0x96, 0xB9, 0xA3, 0xFF, 0x00, 0x00, 0x00, 0x00
                         db   0x00, 0x40, 0x05, 0x0F, 0xFF, 0x00, 0x01, 0x02, 0x03
                         db   0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
                         db   0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00

    ;-------------------;
    ;   VGA palettes    ;
    ;-------------------;

         ; palette256      db   00, 00, 00, 00, 10, 41, 12, 28, 18, 02, 43, 22, 35
         ;                 db   19, 09, 58, 00, 00, 57, 35, 12, 43, 43, 47, 24, 24
         ;                 db   28, 20, 24, 60, 10, 60, 15, 31, 47, 63, 62, 56, 20
         ;                 db   60, 56, 22, 63, 61, 36, 63, 63, 63, 00, 00, 00, 05
         ;                 db   05, 05, 08, 08, 08, 11, 11, 11, 14, 14, 14, 17, 17
         ;                 db   17, 20, 20, 20, 24, 24, 24, 28, 28, 28, 32, 32, 32
         ;                 db   36, 36, 36, 40, 40, 40, 45, 45, 45, 50, 50, 50, 56
         ;                 db   56, 56, 63, 63, 63, 13, 12, 15, 15, 16, 22, 17, 20
         ;                 db   29, 19, 24, 36, 21, 28, 43, 23, 31, 50, 25, 34, 57
         ;                 db   26, 42, 63, 00, 15, 02, 01, 22, 04, 02, 29, 06, 03
         ;                 db   36, 08, 04, 43, 10, 05, 50, 12, 06, 57, 14, 20, 63
         ;                 db   40, 18, 06, 07, 25, 12, 11, 33, 17, 14, 40, 23, 18
         ;                 db   48, 28, 21, 55, 34, 25, 62, 39, 27, 63, 48, 36, 15
         ;                 db   03, 02, 22, 06, 04, 29, 09, 06, 36, 12, 08, 43, 15
         ;                 db   10, 50, 18, 12, 57, 21, 14, 63, 28, 20, 15, 00, 00
         ;                 db   22, 07, 00, 29, 15, 00, 36, 23, 00, 43, 31, 00, 50
         ;                 db   39, 00, 57, 47, 00, 63, 55, 00, 15, 05, 03, 22, 11
         ;                 db   07, 29, 17, 11, 36, 23, 15, 43, 29, 19, 50, 35, 23
         ;                 db   57, 41, 27, 63, 53, 34, 28, 14, 12, 33, 20, 14, 38
         ;                 db   26, 16, 43, 32, 18, 48, 38, 20, 53, 44, 22, 58, 50
         ;                 db   24, 63, 56, 30, 05, 05, 06, 10, 10, 13, 15, 15, 20
         ;                 db   20, 20, 27, 25, 25, 34, 30, 30, 41, 35, 35, 48, 44
         ;                 db   44, 63, 03, 06, 05, 05, 11, 09, 07, 16, 13, 09, 21
         ;                 db   17, 11, 26, 21, 13, 31, 25, 15, 36, 29, 20, 48, 38
         ;                 db   06, 06, 07, 13, 13, 15, 20, 20, 23, 27, 27, 31, 34
         ;                 db   34, 39, 41, 41, 47, 48, 48, 55, 57, 57, 63, 06, 15
         ;                 db   04, 12, 22, 08, 18, 29, 12, 24, 36, 16, 30, 43, 20
         ;                 db   36, 50, 24, 42, 57, 28, 54, 63, 35, 15, 10, 10, 22
         ;                 db   16, 16, 29, 21, 21, 36, 27, 27, 43, 32, 32, 50, 38
         ;                 db   38, 57, 43, 43, 63, 54, 54, 15, 15, 06, 22, 22, 12
         ;                 db   29, 29, 18, 36, 36, 24, 43, 43, 30, 50, 50, 36, 57
         ;                 db   57, 42, 63, 63, 54, 02, 04, 14, 06, 12, 21, 10, 20
         ;                 db   28, 14, 28, 35, 18, 36, 42, 22, 44, 49, 26, 52, 56
         ;                 db   36, 63, 63, 18, 04, 14, 24, 08, 21, 31, 12, 28, 37
         ;                 db   16, 35, 44, 20, 42, 50, 24, 49, 57, 28, 56, 63, 38
         ;                 db   63, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 53, 44, 22, 09
         ;                 db   08, 12, 16, 14, 16, 22, 21, 20, 29, 27, 24, 35, 34
         ;                 db   28, 42, 40, 32, 48, 47, 36, 57, 56, 43, 08, 12, 16
         ;                 db   14, 16, 22, 21, 20, 29, 27, 24, 35, 34, 28, 42, 40
         ;                 db   32, 48, 47, 36, 57, 56, 43, 63, 13, 09, 11, 21, 16
         ;                 db   15, 27, 22, 18, 36, 29, 22, 42, 35, 25, 51, 42, 29
         ;                 db   57, 48, 32, 63, 56, 39, 06, 14, 09, 12, 21, 14, 18
         ;                 db   27, 22, 24, 33, 28, 30, 39, 36, 36, 46, 42, 42, 52
         ;                 db   47, 50, 59, 53, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00
         ;                 db   00

