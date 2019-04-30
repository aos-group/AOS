[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_init_gdtidt
	EXTERN	_init_pic
	EXTERN	_io_sti
	EXTERN	_fifo32_init
	EXTERN	_init_pit
	EXTERN	_init_keyboard
	EXTERN	_enable_mouse
	EXTERN	_io_out8
	EXTERN	_memtest
	EXTERN	_memman
	EXTERN	_memman_init
	EXTERN	_memman_free
	EXTERN	_init_palette
	EXTERN	_shtctl_init
	EXTERN	_task_init
	EXTERN	_task_run
	EXTERN	_sheet_alloc
	EXTERN	_memman_alloc_4k
	EXTERN	_sheet_setbuf
	EXTERN	_init_screen
	EXTERN	_init_mouse_cursor8
	EXTERN	_sheet_slide
	EXTERN	_sheet_updown
	EXTERN	_fifo32_put
	EXTERN	_fifo32_status
	EXTERN	_io_cli
	EXTERN	_fifo32_get
	EXTERN	_mouse_decode
	EXTERN	_cons_putstr0
	EXTERN	_asm_end_app
	EXTERN	_wait_KBC_sendready
	EXTERN	_task_sleep
	EXTERN	_change_wtitle8
	EXTERN	_task_alloc
	EXTERN	_make_window8
	EXTERN	_make_textbox8
	EXTERN	_console_task
	EXTERN	_memman_free_4k
	EXTERN	_sheet_free
[FILE "bootpack.c"]
	GLOBAL	_binfo
[SECTION .data]
	ALIGNB	4
_binfo:
	DD	4080
_keytable0.0:
	DB	0
	DB	0
	DB	49
	DB	50
	DB	51
	DB	52
	DB	53
	DB	54
	DB	55
	DB	56
	DB	57
	DB	48
	DB	45
	DB	94
	DB	8
	DB	0
	DB	81
	DB	87
	DB	69
	DB	82
	DB	84
	DB	89
	DB	85
	DB	73
	DB	79
	DB	80
	DB	64
	DB	91
	DB	10
	DB	0
	DB	65
	DB	83
	DB	68
	DB	70
	DB	71
	DB	72
	DB	74
	DB	75
	DB	76
	DB	59
	DB	58
	DB	0
	DB	0
	DB	93
	DB	90
	DB	88
	DB	67
	DB	86
	DB	66
	DB	78
	DB	77
	DB	44
	DB	46
	DB	47
	DB	0
	DB	42
	DB	0
	DB	32
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	55
	DB	56
	DB	57
	DB	45
	DB	52
	DB	53
	DB	54
	DB	43
	DB	49
	DB	50
	DB	51
	DB	48
	DB	46
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	92
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	92
	DB	0
	DB	0
_keytable1.1:
	DB	0
	DB	0
	DB	33
	DB	34
	DB	35
	DB	36
	DB	37
	DB	38
	DB	39
	DB	40
	DB	41
	DB	126
	DB	61
	DB	126
	DB	8
	DB	0
	DB	81
	DB	87
	DB	69
	DB	82
	DB	84
	DB	89
	DB	85
	DB	73
	DB	79
	DB	80
	DB	96
	DB	123
	DB	10
	DB	0
	DB	65
	DB	83
	DB	68
	DB	70
	DB	71
	DB	72
	DB	74
	DB	75
	DB	76
	DB	43
	DB	42
	DB	0
	DB	0
	DB	125
	DB	90
	DB	88
	DB	67
	DB	86
	DB	66
	DB	78
	DB	77
	DB	60
	DB	62
	DB	63
	DB	0
	DB	42
	DB	0
	DB	32
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	55
	DB	56
	DB	57
	DB	45
	DB	52
	DB	53
	DB	54
	DB	43
	DB	49
	DB	50
	DB	51
	DB	48
	DB	46
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	95
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	0
	DB	124
	DB	0
	DB	0
LC1:
	DB	0x0A,"Break(mouse):",0x0A,0x00
LC0:
	DB	0x0A,"Break(key):",0x0A,0x00
[SECTION .text]
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,1100
	MOV	EDX,DWORD [_binfo]
	MOV	DWORD [-1048+EBP],-1
	MOVSX	EAX,WORD [4+EDX]
	SUB	EAX,16
	SAR	EAX,1
	MOV	DWORD [-1040+EBP],EAX
	MOVSX	EAX,WORD [6+EDX]
	SUB	EAX,44
	SAR	EAX,1
	MOV	DWORD [-1044+EBP],EAX
	MOV	DWORD [-1052+EBP],0
	MOV	DWORD [-1056+EBP],2147483647
	MOV	DWORD [-1060+EBP],0
	MOV	DWORD [-1080+EBP],0
	MOV	AL,BYTE [1+EDX]
	SAR	AL,4
	MOV	EDX,EAX
	AND	EDX,7
	MOV	DWORD [-1084+EBP],EDX
	MOV	DWORD [-1088+EBP],-1
	MOV	DWORD [-1092+EBP],-1
	MOV	DWORD [-1096+EBP],-1
	MOV	DWORD [-1100+EBP],0
	MOV	DWORD [-1104+EBP],0
	CALL	_init_gdtidt
	CALL	_init_pic
	CALL	_io_sti
	LEA	EAX,DWORD [-636+EBP]
	PUSH	0
	PUSH	EAX
	LEA	EAX,DWORD [-44+EBP]
	PUSH	128
	PUSH	EAX
	CALL	_fifo32_init
	LEA	EDX,DWORD [-76+EBP]
	PUSH	0
	LEA	EAX,DWORD [-764+EBP]
	PUSH	EAX
	PUSH	32
	PUSH	EDX
	CALL	_fifo32_init
	ADD	ESP,32
	CALL	_init_pit
	LEA	EAX,DWORD [-44+EBP]
	PUSH	256
	PUSH	EAX
	CALL	_init_keyboard
	LEA	EDX,DWORD [-44+EBP]
	LEA	EAX,DWORD [-780+EBP]
	PUSH	EAX
	PUSH	512
	PUSH	EDX
	CALL	_enable_mouse
	PUSH	248
	PUSH	33
	CALL	_io_out8
	PUSH	239
	PUSH	161
	CALL	_io_out8
	ADD	ESP,36
	PUSH	-1073741825
	PUSH	4194304
	CALL	_memtest
	PUSH	DWORD [_memman]
	MOV	DWORD [-1064+EBP],EAX
	CALL	_memman_init
	PUSH	647168
	PUSH	4096
	PUSH	DWORD [_memman]
	CALL	_memman_free
	MOV	EAX,DWORD [-1064+EBP]
	SUB	EAX,4194304
	PUSH	EAX
	PUSH	4194304
	PUSH	DWORD [_memman]
	CALL	_memman_free
	ADD	ESP,36
	CALL	_init_palette
	MOV	EDX,DWORD [_binfo]
	MOVSX	EAX,WORD [6+EDX]
	PUSH	EAX
	MOVSX	EAX,WORD [4+EDX]
	PUSH	EAX
	PUSH	DWORD [8+EDX]
	PUSH	DWORD [_memman]
	CALL	_shtctl_init
	MOV	DWORD [4068],EAX
	PUSH	DWORD [_memman]
	MOV	DWORD [-1068+EBP],EAX
	CALL	_task_init
	PUSH	2
	PUSH	1
	MOV	DWORD [-1076+EBP],EAX
	PUSH	EAX
	MOV	DWORD [-20+EBP],EAX
	CALL	_task_run
	ADD	ESP,32
	PUSH	DWORD [-1068+EBP]
	CALL	_sheet_alloc
	MOV	EDX,DWORD [_binfo]
	MOV	ESI,EAX
	MOVSX	EAX,WORD [4+EDX]
	MOVSX	EDX,WORD [6+EDX]
	IMUL	EAX,EDX
	PUSH	EAX
	PUSH	DWORD [_memman]
	CALL	_memman_alloc_4k
	MOV	EDX,DWORD [_binfo]
	PUSH	-1
	MOV	EBX,EAX
	MOVSX	EAX,WORD [6+EDX]
	PUSH	EAX
	MOVSX	EAX,WORD [4+EDX]
	PUSH	EAX
	PUSH	EBX
	PUSH	ESI
	CALL	_sheet_setbuf
	MOV	EDX,DWORD [_binfo]
	ADD	ESP,32
	MOVSX	EAX,WORD [6+EDX]
	PUSH	EAX
	MOVSX	EAX,WORD [4+EDX]
	PUSH	EAX
	PUSH	EBX
	LEA	EBX,DWORD [-1036+EBP]
	CALL	_init_screen
	PUSH	DWORD [-1064+EBP]
	PUSH	DWORD [-1068+EBP]
	CALL	_open_console
	PUSH	DWORD [-1068+EBP]
	MOV	EDI,EAX
	CALL	_sheet_alloc
	PUSH	99
	PUSH	16
	MOV	DWORD [-1072+EBP],EAX
	PUSH	16
	PUSH	EBX
	PUSH	EAX
	CALL	_sheet_setbuf
	ADD	ESP,44
	PUSH	99
	PUSH	EBX
	CALL	_init_mouse_cursor8
	PUSH	0
	PUSH	0
	PUSH	ESI
	CALL	_sheet_slide
	PUSH	4
	PUSH	32
	PUSH	EDI
	CALL	_sheet_slide
	ADD	ESP,32
	PUSH	DWORD [-1044+EBP]
	PUSH	DWORD [-1040+EBP]
	PUSH	DWORD [-1072+EBP]
	CALL	_sheet_slide
	PUSH	0
	PUSH	ESI
	CALL	_sheet_updown
	PUSH	1
	PUSH	EDI
	CALL	_sheet_updown
	PUSH	2
	PUSH	DWORD [-1072+EBP]
	CALL	_sheet_updown
	ADD	ESP,36
	PUSH	EDI
	CALL	_keywin_on
	LEA	EAX,DWORD [-76+EBP]
	PUSH	237
	PUSH	EAX
	CALL	_fifo32_put
	LEA	EDX,DWORD [-76+EBP]
	PUSH	DWORD [-1084+EBP]
	PUSH	EDX
	CALL	_fifo32_put
	ADD	ESP,20
	LEA	EAX,DWORD [-44+EBP]
	MOV	DWORD [4076],EAX
L80:
	LEA	EBX,DWORD [-76+EBP]
	PUSH	EBX
	CALL	_fifo32_status
	POP	ECX
	TEST	EAX,EAX
	JLE	L5
	CMP	DWORD [-1088+EBP],0
	JS	L93
L5:
	LEA	EBX,DWORD [-44+EBP]
	CALL	_io_cli
	PUSH	EBX
	CALL	_fifo32_status
	POP	EDX
	TEST	EAX,EAX
	JE	L94
	PUSH	EBX
	CALL	_fifo32_get
	MOV	EBX,EAX
	CALL	_io_sti
	POP	EAX
	TEST	EDI,EDI
	JE	L12
	CMP	DWORD [28+EDI],0
	JNE	L12
	MOV	EDX,DWORD [-1068+EBP]
	XOR	EDI,EDI
	MOV	EAX,DWORD [16+EDX]
	CMP	EAX,1
	JE	L12
	MOV	EDX,DWORD [-1068+EBP]
	MOV	EDI,DWORD [16+EDX+EAX*4]
	PUSH	EDI
	CALL	_keywin_on
	POP	EAX
L12:
	LEA	EAX,DWORD [-256+EBX]
	CMP	EAX,255
	JBE	L95
	LEA	EAX,DWORD [-512+EBX]
	CMP	EAX,255
	JBE	L96
	LEA	EAX,DWORD [-768+EBX]
	CMP	EAX,255
	JA	L80
	IMUL	EAX,EBX,40
	MOV	EDX,DWORD [-1068+EBP]
	LEA	EAX,DWORD [-29676+EAX+EDX*1]
	PUSH	EAX
	CALL	_close_console
L86:
	POP	EAX
	JMP	L80
L96:
	MOVZX	EAX,BL
	PUSH	EAX
	LEA	EAX,DWORD [-780+EBP]
	PUSH	EAX
	CALL	_mouse_decode
	POP	EDX
	POP	ECX
	TEST	EAX,EAX
	JE	L80
	MOV	EAX,DWORD [-772+EBP]
	MOV	EDX,DWORD [-776+EBP]
	ADD	DWORD [-1044+EBP],EAX
	ADD	DWORD [-1040+EBP],EDX
	JS	L97
L57:
	CMP	DWORD [-1044+EBP],0
	JS	L98
L58:
	MOV	EDX,DWORD [_binfo]
	MOVSX	EAX,WORD [4+EDX]
	DEC	EAX
	CMP	DWORD [-1040+EBP],EAX
	JLE	L59
	MOV	DWORD [-1040+EBP],EAX
L59:
	MOVSX	EAX,WORD [6+EDX]
	DEC	EAX
	CMP	DWORD [-1044+EBP],EAX
	JLE	L60
	MOV	DWORD [-1044+EBP],EAX
L60:
	MOV	EAX,DWORD [-1040+EBP]
	MOV	EDX,DWORD [-1044+EBP]
	MOV	DWORD [-1048+EBP],EAX
	MOV	DWORD [-1052+EBP],EDX
	TEST	DWORD [-768+EBP],1
	JNE	L99
	MOV	DWORD [-1092+EBP],-1
	CMP	DWORD [-1056+EBP],2147483647
	JE	L80
L87:
	PUSH	DWORD [-1060+EBP]
	PUSH	DWORD [-1056+EBP]
	PUSH	DWORD [-1104+EBP]
	CALL	_sheet_slide
	MOV	DWORD [-1056+EBP],2147483647
L88:
	ADD	ESP,12
	JMP	L80
L99:
	CMP	DWORD [-1092+EBP],0
	JS	L100
	MOV	ESI,DWORD [-1040+EBP]
	MOV	EAX,DWORD [-1100+EBP]
	SUB	ESI,DWORD [-1092+EBP]
	MOV	EBX,DWORD [-1044+EBP]
	SUB	EBX,DWORD [-1096+EBP]
	MOV	EDX,DWORD [-1044+EBP]
	LEA	ESI,DWORD [2+ESI+EAX*1]
	MOV	DWORD [-1096+EBP],EDX
	MOV	DWORD [-1056+EBP],ESI
	ADD	DWORD [-1060+EBP],EBX
	AND	DWORD [-1056+EBP],-4
	JMP	L80
L100:
	MOV	EAX,DWORD [-1068+EBP]
	MOV	EAX,DWORD [16+EAX]
	MOV	DWORD [-1108+EBP],EAX
	MOV	ECX,EAX
L90:
	DEC	ECX
	TEST	ECX,ECX
	JLE	L80
	MOV	EDX,DWORD [-1068+EBP]
	MOV	EBX,DWORD [-1044+EBP]
	MOV	ESI,DWORD [-1040+EBP]
	MOV	EDX,DWORD [20+EDX+ECX*4]
	MOV	DWORD [-1104+EBP],EDX
	SUB	EBX,DWORD [16+EDX]
	SUB	ESI,DWORD [12+EDX]
	JS	L90
	MOV	EAX,DWORD [4+EDX]
	CMP	ESI,EAX
	JGE	L90
	TEST	EBX,EBX
	JS	L90
	CMP	EBX,DWORD [8+EDX]
	JGE	L90
	IMUL	EAX,EBX
	MOV	DWORD [-1112+EBP],EAX
	MOV	EDX,DWORD [EDX]
	MOV	EAX,DWORD [-1112+EBP]
	ADD	EAX,EBX
	MOVZX	EAX,BYTE [EAX+EDX*1]
	MOV	EDX,DWORD [-1104+EBP]
	CMP	EAX,DWORD [20+EDX]
	JE	L90
	MOV	EAX,DWORD [-1108+EBP]
	DEC	EAX
	PUSH	EAX
	PUSH	EDX
	CALL	_sheet_updown
	CMP	DWORD [-1104+EBP],EDI
	POP	ECX
	POP	EAX
	JE	L69
	PUSH	EDI
	CALL	_keywin_off
	MOV	EDI,DWORD [-1104+EBP]
	PUSH	EDI
	CALL	_keywin_on
	POP	EAX
	POP	EDX
L69:
	CMP	ESI,2
	JLE	L70
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EAX,DWORD [4+EDX]
	SUB	EAX,3
	CMP	ESI,EAX
	JGE	L70
	CMP	EBX,2
	JLE	L70
	CMP	EBX,20
	JG	L70
	MOV	EAX,DWORD [-1040+EBP]
	MOV	EDX,DWORD [-1044+EBP]
	MOV	DWORD [-1092+EBP],EAX
	MOV	DWORD [-1096+EBP],EDX
	MOV	EAX,DWORD [-1104+EBP]
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EAX,DWORD [12+EAX]
	MOV	EDX,DWORD [16+EDX]
	MOV	DWORD [-1100+EBP],EAX
	MOV	DWORD [-1060+EBP],EDX
L70:
	MOV	EAX,DWORD [-1104+EBP]
	MOV	EDX,DWORD [4+EAX]
	LEA	EAX,DWORD [-21+EDX]
	CMP	EAX,ESI
	JG	L80
	LEA	EAX,DWORD [-5+EDX]
	CMP	ESI,EAX
	JGE	L80
	CMP	EBX,4
	JLE	L80
	CMP	EBX,18
	JG	L80
	MOV	EDX,DWORD [-1104+EBP]
	TEST	BYTE [28+EDX],16
	JE	L101
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EBX,DWORD [36+EDX]
	PUSH	LC1
L92:
	PUSH	DWORD [148+EBX]
	CALL	_cons_putstr0
	CALL	_io_cli
	LEA	EAX,DWORD [48+EBX]
	MOV	DWORD [84+EBX],EAX
	MOV	DWORD [76+EBX],_asm_end_app
L91:
	CALL	_io_sti
L84:
	POP	EDX
	POP	ECX
	JMP	L80
L101:
	MOV	EBX,DWORD [36+EDX]
	CALL	_io_cli
	PUSH	4
	LEA	EAX,DWORD [16+EBX]
	PUSH	EAX
	CALL	_fifo32_put
	JMP	L91
L98:
	MOV	DWORD [-1044+EBP],0
	JMP	L58
L97:
	MOV	DWORD [-1040+EBP],0
	JMP	L57
L95:
	CMP	EBX,383
	JG	L16
	CMP	DWORD [-1080+EBP],0
	JE	L17
	MOV	AL,BYTE [_keytable1.1-256+EBX]
L89:
	MOV	BYTE [-124+EBP],AL
L19:
	MOV	DL,BYTE [-124+EBP]
	LEA	EAX,DWORD [-65+EDX]
	CMP	AL,25
	JA	L20
	TEST	DWORD [-1084+EBP],4
	JNE	L82
	CMP	DWORD [-1080+EBP],0
	JE	L22
L20:
	MOV	AL,BYTE [-124+EBP]
	TEST	AL,AL
	JE	L24
	TEST	EDI,EDI
	JNE	L102
L24:
	CMP	EBX,271
	JNE	L26
	TEST	EDI,EDI
	JNE	L103
L26:
	CMP	EBX,298
	JE	L104
	CMP	EBX,310
	JE	L105
	CMP	EBX,426
	JE	L106
	CMP	EBX,438
	JE	L107
	CMP	EBX,314
	JE	L108
	CMP	EBX,325
	JE	L109
	CMP	EBX,326
	JE	L110
	CMP	EBX,506
	JE	L111
	CMP	EBX,510
	JE	L112
	CMP	EBX,315
	JNE	L47
	CMP	DWORD [-1080+EBP],0
	JE	L47
	TEST	EDI,EDI
	JNE	L113
L47:
	CMP	EBX,316
	JNE	L50
	CMP	DWORD [-1080+EBP],0
	JNE	L114
L50:
	CMP	EBX,343
	JNE	L80
	MOV	EDX,DWORD [-1068+EBP]
	MOV	EAX,DWORD [16+EDX]
	CMP	EAX,2
	JLE	L80
	DEC	EAX
	PUSH	EAX
	PUSH	DWORD [24+EDX]
	CALL	_sheet_updown
	JMP	L84
L114:
	TEST	EDI,EDI
	JE	L51
	PUSH	EDI
	CALL	_keywin_off
	POP	EBX
L51:
	PUSH	DWORD [-1064+EBP]
	PUSH	DWORD [-1068+EBP]
	CALL	_open_console
	PUSH	4
	PUSH	32
	MOV	EDI,EAX
	PUSH	EAX
	CALL	_sheet_slide
	MOV	EAX,DWORD [-1068+EBP]
	PUSH	DWORD [16+EAX]
	PUSH	EDI
	CALL	_sheet_updown
	PUSH	EDI
	CALL	_keywin_on
	ADD	ESP,32
	JMP	L80
L113:
	MOV	EBX,DWORD [36+EDI]
	TEST	EBX,EBX
	JE	L80
	CMP	DWORD [52+EBX],0
	JE	L80
	PUSH	LC0
	JMP	L92
L112:
	CALL	_wait_KBC_sendready
	PUSH	DWORD [-1088+EBP]
	PUSH	96
	CALL	_io_out8
	JMP	L84
L111:
	MOV	DWORD [-1088+EBP],-1
	JMP	L80
L110:
	XOR	DWORD [-1084+EBP],1
L85:
	PUSH	237
	LEA	EBX,DWORD [-76+EBP]
	PUSH	EBX
	CALL	_fifo32_put
	PUSH	DWORD [-1084+EBP]
	PUSH	EBX
	CALL	_fifo32_put
	ADD	ESP,16
	JMP	L80
L109:
	XOR	DWORD [-1084+EBP],2
	JMP	L85
L108:
	XOR	DWORD [-1084+EBP],4
	JMP	L85
L107:
	AND	DWORD [-1080+EBP],-3
	JMP	L80
L106:
	AND	DWORD [-1080+EBP],-2
	JMP	L80
L105:
	OR	DWORD [-1080+EBP],2
	JMP	L80
L104:
	OR	DWORD [-1080+EBP],1
	JMP	L80
L103:
	PUSH	EDI
	CALL	_keywin_off
	MOV	ECX,DWORD [24+EDI]
	DEC	ECX
	POP	ESI
	JNE	L27
	MOV	EAX,DWORD [-1068+EBP]
	MOV	ECX,DWORD [16+EAX]
	DEC	ECX
L27:
	MOV	EDX,DWORD [-1068+EBP]
	MOV	EDI,DWORD [20+EDX+ECX*4]
	PUSH	EDI
	CALL	_keywin_on
	JMP	L86
L102:
	MOVSX	EAX,AL
	ADD	EAX,256
	PUSH	EAX
	MOV	EAX,DWORD [36+EDI]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	JMP	L84
L22:
	LEA	EAX,DWORD [32+EDX]
	MOV	BYTE [-124+EBP],AL
	JMP	L20
L82:
	CMP	DWORD [-1080+EBP],0
	JE	L20
	JMP	L22
L17:
	MOV	AL,BYTE [_keytable0.0-256+EBX]
	JMP	L89
L16:
	MOV	BYTE [-124+EBP],0
	JMP	L19
L94:
	CMP	DWORD [-1048+EBP],0
	JNS	L115
	CMP	DWORD [-1056+EBP],2147483647
	JNE	L116
	PUSH	DWORD [-1076+EBP]
	CALL	_task_sleep
	CALL	_io_sti
	JMP	L86
L116:
	CALL	_io_sti
	JMP	L87
L115:
	CALL	_io_sti
	PUSH	DWORD [-1052+EBP]
	PUSH	DWORD [-1048+EBP]
	PUSH	DWORD [-1072+EBP]
	CALL	_sheet_slide
	MOV	DWORD [-1048+EBP],-1
	JMP	L88
L93:
	PUSH	EBX
	CALL	_fifo32_get
	MOV	DWORD [-1088+EBP],EAX
	CALL	_wait_KBC_sendready
	PUSH	DWORD [-1088+EBP]
	PUSH	96
	CALL	_io_out8
	ADD	ESP,12
	JMP	L5
	GLOBAL	_keywin_off
_keywin_off:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	0
	MOV	EBX,DWORD [8+EBP]
	PUSH	EBX
	CALL	_change_wtitle8
	TEST	BYTE [28+EBX],32
	POP	EAX
	POP	EDX
	JNE	L119
L117:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L119:
	PUSH	3
	MOV	EAX,DWORD [36+EBX]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	POP	EBX
	POP	EAX
	JMP	L117
	GLOBAL	_keywin_on
_keywin_on:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	1
	MOV	EBX,DWORD [8+EBP]
	PUSH	EBX
	CALL	_change_wtitle8
	TEST	BYTE [28+EBX],32
	POP	EAX
	POP	EDX
	JNE	L122
L120:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L122:
	PUSH	2
	MOV	EAX,DWORD [36+EBX]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	POP	ECX
	POP	EBX
	JMP	L120
[SECTION .data]
LC2:
	DB	"console",0x00
[SECTION .text]
	GLOBAL	_open_console
_open_console:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	PUSH	ECX
	PUSH	DWORD [8+EBP]
	CALL	_sheet_alloc
	PUSH	42240
	PUSH	DWORD [_memman]
	MOV	EBX,EAX
	CALL	_memman_alloc_4k
	MOV	ESI,EAX
	CALL	_task_alloc
	PUSH	512
	MOV	EDI,EAX
	PUSH	DWORD [_memman]
	CALL	_memman_alloc_4k
	PUSH	-1
	PUSH	165
	PUSH	256
	MOV	DWORD [-16+EBP],EAX
	PUSH	ESI
	PUSH	EBX
	CALL	_sheet_setbuf
	ADD	ESP,40
	PUSH	0
	PUSH	LC2
	PUSH	165
	PUSH	256
	PUSH	ESI
	CALL	_make_window8
	PUSH	0
	PUSH	128
	PUSH	240
	PUSH	28
	PUSH	8
	PUSH	EBX
	CALL	_make_textbox8
	ADD	ESP,44
	PUSH	65536
	PUSH	DWORD [_memman]
	CALL	_memman_alloc_4k
	MOV	DWORD [76+EDI],_console_task
	MOV	DWORD [156+EDI],EAX
	ADD	EAX,65524
	MOV	DWORD [100+EDI],EAX
	MOV	DWORD [116+EDI],8
	MOV	DWORD [120+EDI],16
	MOV	DWORD [124+EDI],8
	MOV	DWORD [128+EDI],8
	MOV	DWORD [132+EDI],8
	MOV	DWORD [136+EDI],8
	MOV	DWORD [4+EAX],EBX
	MOV	EDX,DWORD [100+EDI]
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [8+EDX],EAX
	PUSH	2
	PUSH	2
	PUSH	EDI
	CALL	_task_run
	LEA	EAX,DWORD [16+EDI]
	MOV	DWORD [36+EBX],EDI
	OR	DWORD [28+EBX],32
	PUSH	EDI
	PUSH	DWORD [-16+EBP]
	PUSH	128
	PUSH	EAX
	CALL	_fifo32_init
	LEA	ESP,DWORD [-12+EBP]
	MOV	EAX,EBX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_close_constask
_close_constask:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	PUSH	EBX
	CALL	_task_sleep
	PUSH	65536
	PUSH	DWORD [156+EBX]
	PUSH	DWORD [_memman]
	CALL	_memman_free_4k
	PUSH	512
	PUSH	DWORD [16+EBX]
	PUSH	DWORD [_memman]
	CALL	_memman_free_4k
	MOV	DWORD [4+EBX],0
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
	GLOBAL	_close_console
_close_console:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	ESI,DWORD [36+EBX]
	PUSH	42240
	PUSH	DWORD [EBX]
	PUSH	DWORD [_memman]
	CALL	_memman_free_4k
	PUSH	EBX
	CALL	_sheet_free
	MOV	DWORD [8+EBP],ESI
	ADD	ESP,16
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	JMP	_close_constask
