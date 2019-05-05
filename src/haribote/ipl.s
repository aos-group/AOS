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
	times 18 db 0
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
	mov bx, 18 * 2 * CYLS - 1	; 要读取的合计扇区数
	call readfast

; 读取结束，运行 haribote.sys
	mov byte [0x0ff0], CYLS		; ipl 读取结束时的扇区号
	jmp 0xc400

error:
	mov ax, 0
	mov es, ax
	mov si, msg
putloop:
	mov al, [si]			; 待打印的字符
	inc si
	cmp al, 0
	je 	fin
	mov ah, 0x0e			; 显示一个字符
	mov bx, 15				; 指定字符颜色
	int 0x10				; 调用显卡 BIOS
	jmp putloop

fin:
	hlt
	jmp fin

; 信息显示部分
msg:
	db 	0x0a, 0x0a	; 换行
	db	"load error"
	db	0x0a
	db	0

readfast:	; 使用 al 尽量一次性读取数据
; es: 读取地址，ch: 柱面，dh: 磁头，cl: 扇区，bx: 读取扇区数
	mov ax, es 				; <通过 es 计算 al 的最大值>
	shl ax, 3				; 将 ax 除以 32, 将结果存入 ah (16 * 32 = 512) (将 es 由 16 字节为单位转换为 512 字节为单位)
	and ah, 0x7f 			; ah 是 ah 除以 128 所得的余数 (512 * 128 = 64K, 128 个扇区)
	mov al, 128				; al = 128 - ah; (为了下一次使 es 64 KB 对齐，读取剩下的部分，即不能跨越 64KB 的分块)
	sub al, ah 				; al = 128 - ((es / 32) % 128);

	mov ah, bl 				; <通过 bx 计算 al 的最大值并存入 ah>
	cmp bh, 0 				; if (bh != 0) ah = 18; (如果读取扇区数大于 0xFF，bh > 0，需要通过 bh 和 bl 共同决定最大读取扇区数，如下情况： bh > 0, bl < 18 ==> ah = 18)
	je .skip1
	mov ah, 18
.skip1:
	cmp al, ah 				; al = min(al, ah);
	jbe .skip2
	mov al, ah
.skip2:
	mov ah, 19 				; <通过 cl 计算 al 的最大值并存入 ah>
	sub ah, cl 				; ah = 19 - cl; (由于不能跨越多个磁道，因此最多能一次读到该磁道剩下的扇区)
	cmp al, ah 				; al = min(al, ah);
	jbe .skip3
	mov al, ah
.skip3:
	push bx
	mov si, 0 				; 计算失败次数的寄存器
retry:
	mov ah, 0x02 			; 读盘
	mov bx, 0				; es:bx = 缓冲区地址
	mov dl, 0x00 			; A 驱动器
	push es
	push dx
	push cx
	push ax
	int 0x13 				; 调用磁盘 BIOS
	jnc next
	inc si
	cmp si, 5
	jae error
	mov ah, 0x00
	mov dl, 0x00
	int 0x13				; 重置驱动器
	pop ax
	pop cx
	pop dx
	pop es
	jmp retry
next:
	pop ax
	pop cx
	pop dx
	pop bx 					; 将 es 的内容存入 bx
	shr bx, 5 				; 将 bx 由 16 字节为单位转换为 512 字节为单位
	mov ah, 0
	add bx, ax 				; bx += al
	shl bx, 5 				; 将 bx 由 512 字节为单位转换为 16 字节为单位
	mov es, bx 				; 相当于 es += al * 0x20
	pop bx
	sub bx, ax
	jz .ret
	add cl, al
	cmp cl, 18
	jbe readfast
	mov cl, 1
	inc dh
	cmp dh, 2
	jb 	readfast
	mov dh, 0
	inc ch
	jmp readfast

.ret:
	ret

	times 510-($-$$) db 0
	db	0x55, 0xaa
