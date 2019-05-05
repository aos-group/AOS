; hello-os
; TAB=4

	CYLS	equ		20		; �������������

	org	0x7c00				; ָ�������װ�ص�ַ

; ��������Ǳ�׼ FAT12 ��ʽ����ר�õĴ���
	
	jmp	entry
	db	0x90
	db	"HARIBOTE"			; �����������ƣ�8�ֽ�
	dw	512					; ������С/�ֽ�
	db	1					; ��(�������С��λ)�Ĵ�С
	dw	1					; FAT ����ʼλ�� (һ��ӵ�һ��������ʼ)
			; ��һ�н���
	db	2					; FAT �ĸ��� (�̶�)
	dw	224					; ��Ŀ¼�Ĵ�С/�� (Ĭ��)
	dw	2880				; ���̵Ĵ�С/���� (�̶�)
	db	0xf0 				; ���̵����� (�̶�)
	dw	9					; FAT �ĳ���/���� (�̶�)
	dw	18					; 1 ���ŵ��������� (�̶�)
	dw	2					; ��ͷ�� (�̶�)
	dd	0					; ��ʹ�÷���
			; �ڶ��н���
	dd	2880				; ��дһ�δ��̴�С
	db	0x00, 0x00, 0x29	; �̶�
	dd	0xffffffff			; ������
	db	"HARIBOTEOS "		; ���̵����� (11 �ֽ�)
	db	"FAT12   "			; ���̸�ʽ���� (8 �ֽ�)
	resb	18
			; �����н���

; ��������

entry:
	mov	ax, 0
	mov ss, ax
	mov sp, 0x7c00
	mov ds, ax
	
; ������
; ����ֵ��flags.cf == 0: û�д���, ah == 0
;		 flags.cf == 1: �д���, ah == �����
	mov ax, 0x0820
	mov es, ax
	mov ch, 0				; ���� һ�����
	mov dh, 0				; ��ͷ �������
	mov cl, 2				; ���� �������
readloop:
	mov si, 0				; ��¼ʧ�ܴ����ļĴ���
retry:
	mov ah, 0x2 			; ����
	mov al, 1				; ������������
	mov bx, 0				; es:bx = ��������ַ
	mov dl, 0x00 			; A ������
	int 0x13 				; ���ô��� BIOS
	jnc next
	inc si
	cmp si, 5
	jae error
	mov ah, 0x00
	mov dl, 0x00
	int 0x13				; ����������
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

	mov [0x0ff0], ch		; ipl ��ȡ����ʱ��������
	jmp 0xc200

fin:
	hlt
	jmp fin

error:
	mov si, msg
putloop:
	mov al, [si]			; ����ӡ���ַ�
	inc si
	cmp al, 0
	je 	fin
	mov ah, 0x0e			; ��ʾһ���ַ�
	mov bx, 2				; ָ���ַ���ɫ
	int 0x10				; �����Կ� BIOS
	jmp putloop

; ��Ϣ��ʾ����
msg:
	db 	0x0a, 0x0a	; ����
	db	"load error"
	db	0x0a
	db	0
	resb	0x7dfe - $		; 0x7dfe = 0x7c00 + 0x1fe
	db	0x55, 0xaa