; 有关 BOOT_INFO
	CYLS	equ		0x0ff0			; 设定启动区
	LEDS	equ		0x0ff1
	VMODE	equ		0x0ff2			; 关于颜色数目的信息、颜色的位数
	SCRNX	equ 	0x0ff4			; 分辨率的 X (xcreen x)
	SCRNY	equ		0x0ff6			; 分辨率的 Y (xcreen y)
	VRAM	equ		0x0ff8			; 图像缓冲区的开始地址

	org 0xc200			; 0x8000 + 0x4200

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

fin:
	hlt
	jmp fin