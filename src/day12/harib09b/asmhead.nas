	BOTPAK	equ		0x00280000		; bootpack 加载目标
	DSKCAC	equ		0x00100000		; 磁盘缓冲位置
	DSKCAC0	equ		0x00008000		; 磁盘缓冲位置 (实模式)

; 有关 BOOT_INFO
	CYLS	equ		0x0ff0			; ipl 读取结束时的扇区号的存储地址
	LEDS	equ		0x0ff1
	VMODE	equ		0x0ff2			; 关于颜色数目的信息、颜色的位数
	SCRNX	equ 	0x0ff4			; 分辨率的 X (xcreen x)
	SCRNY	equ		0x0ff6			; 分辨率的 Y (xcreen y)
	VRAM	equ		0x0ff8			; 图像缓冲区的开始地址

	org 0xc200			; 0x8000 + 0x4200

; 设置屏幕模式
	mov al, 0x13		; VGA 显卡，320*200*8 位彩色
	mov ah, 0x00
	int 0x10
	mov byte [VMODE], 8		; 记录画面模式
	mov word [SCRNX], 320
	mov word [SCRNY], 200
	mov dword [VRAM], 0x000a0000

; 用 BIOS 取得键盘上各种 LED 指示灯的状态
	mov ah, 0x02
	int 0x16
	mov [LEDS], al

; PIC 关闭一切中断
; 	根据 AT 兼容机的规格，如果要初始化 PIC,
;	必须在 cli 之前进行，否则有时会挂起
; 	随后进行 PIC 的初始化
	mov al, 0xff
	out 0x21, al			; 禁止主 PIC 的全部中断
	nop						; 如果连续执行 out 指令，有些机种会无法正常运行
	out 0xa1, al			; 禁止从 PIC 的全部中断

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
[instrset "i486p"]			; 使用 486 指令
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

; bootpack 的转送
; 从 bootpack 的地址开始的 512KB 内容复制到 0x00280000 号地址去
	mov esi, bootpack
	mov edi, BOTPAK
	mov ecx, 512 * 1024 / 4
	call memcpy

; 将启动扇区复制到 1MB 以后的内存
; 占用内存空间: 0x00100000 ~ 0x001001ff
	mov esi, 0x7c000
	mov edi, DSKCAC
	mov ecx, 512 / 4
	call memcpy

; 所有剩下的
; 占用内存空间: 0x00100200 ~ 0x001047ff
	mov esi, DSKCAC0 + 512
	mov edi, DSKCAC + 512
	mov ecx, 0
	mov cl, byte [CYLS]
	imul ecx, 512 * 18 * 2 / 4
	sub ecx, 512 / 4
	call memcpy

; 必须由 asmhead 来完成的工作，至此全部完毕
; 	以后就交由 bootpack 来完成

; bootpack 的启动
	mov ebx, BOTPAK
	mov ecx, [ebx + 16]		; 0x11a8
	add ecx, 3
	shr ecx, 2				; ecx 对 4 向上取整
	jz skip
	mov esi, [ebx + 20]		; 0x10c8
	add esi, ebx
	mov edi, [ebx + 12]		; 0x00310000
	call memcpy				; 将 bootpack.hrb 第 0x10c8 字节开始的 0x11a8 字节复制到 0x00310000 号地址
skip:
	mov esp, [ebx + 12]
	jmp dword 2 * 8: 0x0000001b

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

	alignb 16
GDT0:
	resb	8				; NULL selector
	dw 		0xffff, 0x0000, 0x9200, 0x00cf	; 可以读写的段 (segment) 32bit
	dw 		0xffff, 0x0000, 0x9a28, 0x0047 	; 可以执行的段 (segment) 32bit (bootpack 用)

	dw 		0
GDTR0:
	dw 		8 * 3 - 1
	dd 		GDT0

	alignb 16
bootpack: