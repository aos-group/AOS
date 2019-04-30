; hello-os
; TAB=4

	CYLS	equ		20		; 加载柱面的数量

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
	resb	18
			; 第五行结束

; 程序主体

entry:
	mov	ax, 0
	mov ss, ax
	mov sp, 0x7c00
	mov ds, ax
	
; 读磁盘
; 返回值：flags.cf == 0: 没有错误, ah == 0
;		 flags.cf == 1: 有错误, ah == 错误号
	mov ax, 0x0820
	mov es, ax
	mov ch, 0				; 柱面 一级编号
	mov dh, 0				; 磁头 二级编号
	mov cl, 2				; 扇区 三级编号
readloop:
	mov si, 0				; 记录失败次数的寄存器
retry:
	mov ah, 0x2 			; 读盘
	mov al, 1				; 连续的扇区数
	mov bx, 0				; es:bx = 缓冲区地址
	mov dl, 0x00 			; A 驱动器
	int 0x13 				; 调用磁盘 BIOS
	jnc next
	inc si
	cmp si, 5
	jae error
	mov ah, 0x00
	mov dl, 0x00
	int 0x13				; 重置驱动器
	jmp retry
next:
	mov ax, es
	add ax, 0x0020
	mov es, ax
	inc cl
	cmp cl, 18
	jbe readloop
	mov cl, 1
	inc dh
	cmp dh, 2
	jb 	readloop
	mov dh, 0
	inc ch
	cmp ch, CYLS
	jb 	readloop

	mov [0x0ff0], ch		; ipl 读取结束时的扇区号
	jmp 0xc200

fin:
	hlt
	jmp fin

error:
	mov si, msg
putloop:
	mov al, [si]			; 待打印的字符
	inc si
	cmp al, 0
	je 	fin
	mov ah, 0x0e			; 显示一个字符
	mov bx, 2				; 指定字符颜色
	int 0x10				; 调用显卡 BIOS
	jmp putloop

; 信息显示部分
msg:
	db 	0x0a, 0x0a	; 换行
	db	"load error"
	db	0x0a
	db	0
	resb	0x7dfe - $		; 0x7dfe = 0x7c00 + 0x1fe
	db	0x55, 0xaa