; hello-os
; TAB=4

	SECS	equ		720		; 加载扇区的数量

	VBEMODE	equ		0x105 			; 1024 * 768 * 8bit 颜色

	BOTPAK	equ		0x00280000		; bootpack 加载目标
	DSKCAC	equ		0x00100200		; 磁盘缓冲位置
	DSKCAC0	equ		0x00008000		; 磁盘缓冲位置 (实模式)

; 有关 BOOT_INFO
	CYLS	equ		0x0ff0			; ipl 读取结束时的扇区号的存储地址
	LEDS	equ		0x0ff1

	org	0x7c00				; 指明程序的装载地址

; 以下这段是标准 FAT12 格式软盘专用的代码
	
	jmp	entry
	db	0x90
	db	"HARIBOTE"			; 启动区的名称，8字节
	dw	512					; 扇区大小/字节
	db	1					; 簇(分配的最小单位)的大小
	dw	1					; FAT 的起始位置 (一般从第一个扇区开始)
			; 第一行结束
	db	2					; FAT 的个数 (固定)
	dw	224					; 根目录的大小/项 (默认)
	dw	2880				; 磁盘的大小/扇区 (固定)
	db	0xf0 				; 磁盘的种类 (固定)
	dw	9					; FAT 的长度/扇区 (固定)
	dw	18					; 1 个磁道的扇区数 (固定)
	dw	2					; 磁头数 (固定)
	dd	0					; 不使用分区
			; 第二行结束
	dd	2880				; 重写一次磁盘大小
	db	0x00, 0x00, 0x29	; 固定
	dd	0xffffffff			; 卷标号码
	db	"HARIBOTEOS "		; 磁盘的名称 (11 字节)
	db	"FAT12   "			; 磁盘格式名称 (8 字节)

; 程序主体
entry:
;-----------------------------------------------------------------------------------------------------
;-----------------------------------         关闭一切中断        --------------------------------------
;-----------------------------------------------------------------------------------------------------

; ; 用 BIOS 取得键盘上各种 LED 指示灯的状态
; 	mov ah, 0x02
; 	int 0x16
; 	mov [LEDS], al

; PIC 关闭一切中断
; 	根据 AT 兼容机的规格，如果要初始化 PIC,
;	必须在 cli 之前进行，否则有时会挂起
; 	随后进行 PIC 的初始化
	mov al, 0xff
	out 0x21, al			; 禁止主 PIC 的全部中断
	nop						; 如果连续执行 out 指令，有些机种会无法正常运行
	out 0xa1, al			; 禁止从 PIC 的全部中断

;-----------------------------------------------------------------------------------------------------
;-----------------------------------         进入保护模式        --------------------------------------
;-----------------------------------------------------------------------------------------------------

	cli						; 禁止 CPU 级别的中断

; 为了让 CPU 能够访问 1MB 以上的内存空间，设定 A20 GATE
	call waitkbdout
	mov al, 0xd1
	out 0x64, al
	call waitkbdout
	mov al, 0xdf			; enable A20
	out 0x60, al
	call waitkbdout

; 切换到保护模式
	lgdt [GDTR0]			; 设定临时 GDT
	mov eax, cr0
	and eax, 0x7fffffff		; 禁止分页
	or eax, 0x00000001		; 切换到保护模式
	mov cr0, eax
	jmp pipelineflush

pipelineflush:
	mov ax, 1 * 8
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

;-----------------------------------------------------------------------------------------------------
;-------------------------------------         读取磁盘        ----------------------------------------
;-----------------------------------------------------------------------------------------------------

; 读磁盘
	mov ebx, 1
	mov edi, DSKCAC
readloop:
	call readsect
	inc ebx
	cmp ebx, SECS
	jb readloop

;-----------------------------------------------------------------------------------------------------
;-----------------------------------           内存复制          --------------------------------------
;-----------------------------------------------------------------------------------------------------

; 从 bootpack 的地址开始的 512KB 内容复制到 0x00280000 号地址去
	mov esi, 0x00104400
	mov edi, BOTPAK
	mov ecx, 512 * 1024 / 4
	call memcpy

; bootpack 的启动
	mov ebx, BOTPAK
	mov ecx, [ebx + 16]		; 0x11a8, hrb 文件内数据部分的大小
	add ecx, 3
	shr ecx, 2				; ecx 对 4 向上取整
	jz skip
	mov esi, [ebx + 20]		; 0x10c8, hrb 文件内数据部分从哪里开始
	add esi, ebx
	mov edi, [ebx + 12]		; 0x00310000, ESP 初始值 & 数据部分传送目的地址
	call memcpy				; 将 bootpack.hrb 第 0x10c8 字节开始的 0x11a8 字节复制到 0x00310000 号地址
skip:
	mov esp, [ebx + 12]
	jmp dword 2 * 8: 0x0000001b

waitdisk:					; wait for disk ready
	mov dx, 0x1f7
.wait
	in al, dx
	and al, 0x88	; 第 4 位为 1 表示硬盘控制器已准备好数据传输，第 7 位为 1 表示硬盘忙
	cmp al, 0x08
	jnz .wait
	ret

readsect:
	; edi: 目的地址
	; ebx: 扇区号
	; call waitdisk

; 1. 设置要读取的扇区数
	mov dx, 0x1f2
	mov al, 1	; 读取的扇区数
	out dx, al

; 2. 将 LBA 地址存入 0x1f3 ~ 0x1f6
	mov eax, ebx

	; LBA 地址 7 ~ 0 位写入端口 0x1f3
	mov dx, 0x1f3
	out dx, al

	; LBA 地址 15 ~ 8 位写入端口 0x1f4
	mov dx, 0x1f4
	shr eax, 8
	out dx, al

	; LBA 地址 23 ~ 16 位写入端口 0x1f5
	mov dx, 0x1f5
	shr eax, 8
	out dx, al

	mov dx, 0x1f6
	shr eax, 8
	and al, 0x0f 	; LBA 第 24 ~ 27 位
	or al, 0xe0 	; 设置 7 ~ 4 位为 1110, 表示 LBA 模式
	out dx, al

; 3. 向 0x1f7 端口写入读命令，0x20
	mov dx, 0x1f7
	mov al, 0x20
	out dx, al

; 4. 检测硬盘状态
	call waitdisk

; 5. 从 0x1f0 端口读数据
	mov dx, 0x1f0
	mov cx, 256		; 一个扇区有 512 字节，每次读入一个字，共需 512 / 2 = 256 次
	; cld
	; rep insw
.go_on_read:
	in ax, dx
	mov [edi], ax
	add edi, 2
	loop .go_on_read
	ret

waitkbdout:
	in al, 0x64
	and al, 0x02
	in al, 0x60 			; 空读 (为了清空数据接收缓冲区中的垃圾数据)
	jnz waitkbdout
	ret

memcpy:
	mov eax, [esi]
	add esi, 4
	mov [edi], eax
	add edi, 4
	dec ecx
	jnz memcpy
	ret

	align 16, db 0
GDT0:
	times 8 db 0				; NULL selector
	dw 		0xffff, 0x0000, 0x9200, 0x00cf	; 可以读写的段 (segment) 32bit
	dw 		0xffff, 0x0000, 0x9a28, 0x0047 	; 可以执行的段 (segment) 32bit (bootpack 用)

	dw 		0
GDTR0:
	dw 		8 * 3 - 1
	dd 		GDT0

	times 510-($-$$) db 0
	db	0x55, 0xaa
