	org 0xc200			; 0x8000 + 0x4200

	mov al, 0x13		; VGA 显卡，320*200*8 位彩色
	mov ah, 0x00
	int 0x10

fin:
	hlt
	jmp fin