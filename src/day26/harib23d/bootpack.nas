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
	EXTERN	_make_window8
	EXTERN	_make_textbox8
	EXTERN	_task_alloc
	EXTERN	_console_task
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
LC0:
	DB	"console",0x00
LC2:
	DB	0x0A,"Break(mouse):",0x0A,0x00
LC1:
	DB	0x0A,"Break(key):",0x0A,0x00
[SECTION .text]
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	XOR	EDI,EDI
	PUSH	EBX
	LEA	EBX,DWORD [-44+EBP]
	SUB	ESP,1140
	MOV	EDX,DWORD [_binfo]
	MOV	DWORD [-1084+EBP],-1
	MOVSX	EAX,WORD [4+EDX]
	SUB	EAX,16
	SAR	EAX,1
	MOV	DWORD [-1072+EBP],EAX
	MOVSX	EAX,WORD [6+EDX]
	SUB	EAX,44
	SAR	EAX,1
	MOV	DWORD [-1076+EBP],EAX
	MOV	DWORD [-1092+EBP],2147483647
	MOV	DWORD [-1096+EBP],0
	MOV	DWORD [-1120+EBP],0
	MOV	AL,BYTE [1+EDX]
	SAR	AL,4
	MOV	EDX,EAX
	AND	EDX,7
	MOV	DWORD [-1124+EBP],EDX
	MOV	DWORD [-1128+EBP],-1
	MOV	DWORD [-1132+EBP],-1
	MOV	DWORD [-1136+EBP],-1
	MOV	DWORD [-1140+EBP],0
	MOV	DWORD [-1144+EBP],0
	CALL	_init_gdtidt
	CALL	_init_pic
	CALL	_io_sti
	LEA	EAX,DWORD [-636+EBP]
	PUSH	0
	PUSH	EAX
	PUSH	128
	PUSH	EBX
	CALL	_fifo32_init
	LEA	EAX,DWORD [-764+EBP]
	PUSH	0
	PUSH	EAX
	LEA	EAX,DWORD [-76+EBP]
	PUSH	32
	PUSH	EAX
	CALL	_fifo32_init
	ADD	ESP,32
	CALL	_init_pit
	PUSH	256
	PUSH	EBX
	CALL	_init_keyboard
	LEA	EAX,DWORD [-780+EBP]
	PUSH	EAX
	PUSH	512
	PUSH	EBX
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
	MOV	DWORD [-1100+EBP],EAX
	CALL	_memman_init
	PUSH	647168
	PUSH	4096
	PUSH	DWORD [_memman]
	CALL	_memman_free
	MOV	EAX,DWORD [-1100+EBP]
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
	MOV	DWORD [-1104+EBP],EAX
	CALL	_task_init
	PUSH	2
	PUSH	1
	MOV	DWORD [-1116+EBP],EAX
	PUSH	EAX
	MOV	DWORD [-20+EBP],EAX
	CALL	_task_run
	ADD	ESP,32
	PUSH	DWORD [-1104+EBP]
	CALL	_sheet_alloc
	MOV	EDX,DWORD [_binfo]
	MOV	DWORD [-1108+EBP],EAX
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
	PUSH	DWORD [-1108+EBP]
	CALL	_sheet_setbuf
	MOV	EDX,DWORD [_binfo]
	ADD	ESP,32
	MOVSX	EAX,WORD [6+EDX]
	PUSH	EAX
	MOVSX	EAX,WORD [4+EDX]
	PUSH	EAX
	PUSH	EBX
	CALL	_init_screen
	ADD	ESP,12
	MOV	DWORD [-1080+EBP],1
L6:
	PUSH	DWORD [-1104+EBP]
	LEA	EBX,DWORD [-1052+EBP]
	CALL	_sheet_alloc
	PUSH	42240
	LEA	ESI,DWORD [-1060+EBP]
	PUSH	DWORD [_memman]
	MOV	DWORD [-1044+EBP+EDI*1],EAX
	CALL	_memman_alloc_4k
	MOV	DWORD [EBX+EDI*1],EAX
	PUSH	-1
	PUSH	165
	PUSH	256
	PUSH	EAX
	PUSH	DWORD [-1044+EBP+EDI*1]
	CALL	_sheet_setbuf
	ADD	ESP,32
	PUSH	0
	PUSH	LC0
	PUSH	165
	PUSH	256
	PUSH	DWORD [EBX+EDI*1]
	CALL	_make_window8
	PUSH	0
	PUSH	128
	PUSH	240
	PUSH	28
	PUSH	8
	PUSH	DWORD [-1044+EBP+EDI*1]
	CALL	_make_textbox8
	ADD	ESP,44
	CALL	_task_alloc
	MOV	EBX,EAX
	MOV	DWORD [ESI+EDI*1],EAX
	PUSH	65536
	PUSH	DWORD [_memman]
	CALL	_memman_alloc_4k
	MOV	EDX,DWORD [-1044+EBP+EDI*1]
	ADD	EAX,65524
	MOV	DWORD [100+EBX],EAX
	MOV	EAX,DWORD [ESI+EDI*1]
	MOV	ECX,DWORD [100+EAX]
	MOV	DWORD [76+EAX],_console_task
	MOV	DWORD [116+EAX],8
	MOV	DWORD [120+EAX],16
	MOV	DWORD [124+EAX],8
	MOV	DWORD [128+EAX],8
	MOV	DWORD [132+EAX],8
	MOV	DWORD [136+EAX],8
	MOV	DWORD [4+ECX],EDX
	MOV	ECX,DWORD [-1100+EBP]
	MOV	EDX,DWORD [100+EAX]
	MOV	DWORD [8+EDX],ECX
	PUSH	2
	PUSH	2
	PUSH	EAX
	CALL	_task_run
	MOV	EDX,DWORD [ESI+EDI*1]
	MOV	EAX,DWORD [-1044+EBP+EDI*1]
	OR	DWORD [28+EAX],32
	MOV	DWORD [36+EAX],EDX
	PUSH	512
	PUSH	DWORD [_memman]
	CALL	_memman_alloc_4k
	LEA	EDX,DWORD [-1068+EBP]
	MOV	DWORD [EDX+EDI*1],EAX
	PUSH	DWORD [ESI+EDI*1]
	PUSH	EAX
	PUSH	128
	MOV	EAX,DWORD [ESI+EDI*1]
	ADD	EDI,4
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_init
	ADD	ESP,44
	DEC	DWORD [-1080+EBP]
	JNS	L6
	PUSH	DWORD [-1104+EBP]
	LEA	EBX,DWORD [-1036+EBP]
	CALL	_sheet_alloc
	PUSH	99
	PUSH	16
	MOV	DWORD [-1112+EBP],EAX
	PUSH	16
	PUSH	EBX
	PUSH	EAX
	CALL	_sheet_setbuf
	PUSH	99
	PUSH	EBX
	LEA	EBX,DWORD [-76+EBP]
	CALL	_init_mouse_cursor8
	ADD	ESP,32
	PUSH	0
	PUSH	0
	PUSH	DWORD [-1108+EBP]
	CALL	_sheet_slide
	PUSH	6
	PUSH	56
	PUSH	DWORD [-1040+EBP]
	CALL	_sheet_slide
	PUSH	2
	PUSH	8
	PUSH	DWORD [-1044+EBP]
	CALL	_sheet_slide
	ADD	ESP,36
	PUSH	DWORD [-1076+EBP]
	PUSH	DWORD [-1072+EBP]
	PUSH	DWORD [-1112+EBP]
	CALL	_sheet_slide
	PUSH	0
	PUSH	DWORD [-1108+EBP]
	CALL	_sheet_updown
	PUSH	1
	PUSH	DWORD [-1040+EBP]
	CALL	_sheet_updown
	PUSH	2
	PUSH	DWORD [-1044+EBP]
	CALL	_sheet_updown
	ADD	ESP,36
	PUSH	3
	PUSH	DWORD [-1112+EBP]
	CALL	_sheet_updown
	MOV	EDI,DWORD [-1044+EBP]
	PUSH	EDI
	CALL	_keywin_on
	PUSH	237
	PUSH	EBX
	CALL	_fifo32_put
	PUSH	DWORD [-1124+EBP]
	PUSH	EBX
	CALL	_fifo32_put
	ADD	ESP,28
L77:
	LEA	EBX,DWORD [-76+EBP]
	PUSH	EBX
	CALL	_fifo32_status
	POP	ECX
	TEST	EAX,EAX
	JLE	L10
	CMP	DWORD [-1128+EBP],0
	JS	L90
L10:
	LEA	EBX,DWORD [-44+EBP]
	CALL	_io_cli
	PUSH	EBX
	CALL	_fifo32_status
	POP	EDX
	TEST	EAX,EAX
	JE	L91
	PUSH	EBX
	CALL	_fifo32_get
	MOV	DWORD [-1080+EBP],EAX
	CALL	_io_sti
	POP	EAX
	CMP	DWORD [28+EDI],0
	JNE	L17
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EAX,DWORD [16+EDX]
	MOV	EDI,DWORD [16+EDX+EAX*4]
	PUSH	EDI
	CALL	_keywin_on
	POP	EAX
L17:
	MOV	EAX,DWORD [-1080+EBP]
	SUB	EAX,256
	CMP	EAX,255
	JBE	L92
	MOV	EAX,DWORD [-1080+EBP]
	SUB	EAX,512
	CMP	EAX,255
	JA	L77
	MOVZX	EAX,BYTE [-1080+EBP]
	PUSH	EAX
	LEA	EAX,DWORD [-780+EBP]
	PUSH	EAX
	CALL	_mouse_decode
	POP	ECX
	POP	EBX
	TEST	EAX,EAX
	JE	L77
	MOV	EAX,DWORD [-772+EBP]
	MOV	EDX,DWORD [-776+EBP]
	ADD	DWORD [-1076+EBP],EAX
	ADD	DWORD [-1072+EBP],EDX
	JS	L93
L57:
	CMP	DWORD [-1076+EBP],0
	JS	L94
L58:
	MOV	EDX,DWORD [_binfo]
	MOVSX	EAX,WORD [4+EDX]
	DEC	EAX
	CMP	DWORD [-1072+EBP],EAX
	JLE	L59
	MOV	DWORD [-1072+EBP],EAX
L59:
	MOVSX	EAX,WORD [6+EDX]
	DEC	EAX
	CMP	DWORD [-1076+EBP],EAX
	JLE	L60
	MOV	DWORD [-1076+EBP],EAX
L60:
	MOV	ECX,DWORD [-1072+EBP]
	MOV	EAX,DWORD [-1076+EBP]
	MOV	DWORD [-1084+EBP],ECX
	MOV	DWORD [-1088+EBP],EAX
	TEST	DWORD [-768+EBP],1
	JNE	L95
	MOV	DWORD [-1132+EBP],-1
	CMP	DWORD [-1092+EBP],2147483647
	JE	L77
L85:
	PUSH	DWORD [-1096+EBP]
	PUSH	DWORD [-1092+EBP]
	PUSH	DWORD [-1144+EBP]
	CALL	_sheet_slide
	MOV	DWORD [-1092+EBP],2147483647
L86:
	ADD	ESP,12
	JMP	L77
L95:
	CMP	DWORD [-1132+EBP],0
	JS	L96
	MOV	ESI,DWORD [-1072+EBP]
	MOV	EAX,DWORD [-1140+EBP]
	SUB	ESI,DWORD [-1132+EBP]
	MOV	EBX,DWORD [-1076+EBP]
	SUB	EBX,DWORD [-1136+EBP]
	MOV	EDX,DWORD [-1076+EBP]
	LEA	ESI,DWORD [2+ESI+EAX*1]
	MOV	DWORD [-1136+EBP],EDX
	MOV	DWORD [-1092+EBP],ESI
	ADD	DWORD [-1096+EBP],EBX
	AND	DWORD [-1092+EBP],-4
	JMP	L77
L96:
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EDX,DWORD [16+EDX]
	MOV	DWORD [-1148+EBP],EDX
	MOV	ECX,EDX
L88:
	DEC	ECX
	TEST	ECX,ECX
	JLE	L77
	MOV	EAX,DWORD [-1104+EBP]
	MOV	EBX,DWORD [-1076+EBP]
	MOV	ESI,DWORD [-1072+EBP]
	MOV	EAX,DWORD [20+EAX+ECX*4]
	MOV	DWORD [-1144+EBP],EAX
	SUB	EBX,DWORD [16+EAX]
	SUB	ESI,DWORD [12+EAX]
	JS	L88
	MOV	EAX,DWORD [4+EAX]
	CMP	ESI,EAX
	JGE	L88
	TEST	EBX,EBX
	JS	L88
	MOV	EDX,DWORD [-1144+EBP]
	CMP	EBX,DWORD [8+EDX]
	JGE	L88
	IMUL	EAX,EBX
	MOV	DWORD [-1152+EBP],EAX
	MOV	EDX,DWORD [EDX]
	MOV	EAX,DWORD [-1152+EBP]
	ADD	EAX,EBX
	MOVZX	EAX,BYTE [EAX+EDX*1]
	MOV	EDX,DWORD [-1144+EBP]
	CMP	EAX,DWORD [20+EDX]
	JE	L88
	MOV	EAX,DWORD [-1148+EBP]
	DEC	EAX
	PUSH	EAX
	PUSH	EDX
	CALL	_sheet_updown
	CMP	DWORD [-1144+EBP],EDI
	POP	EAX
	POP	EDX
	JE	L69
	PUSH	EDI
	CALL	_keywin_off
	MOV	EDI,DWORD [-1144+EBP]
	PUSH	EDI
	CALL	_keywin_on
	POP	ECX
	POP	EAX
L69:
	CMP	ESI,2
	JLE	L70
	MOV	ECX,DWORD [-1144+EBP]
	MOV	EAX,DWORD [4+ECX]
	SUB	EAX,3
	CMP	ESI,EAX
	JGE	L70
	CMP	EBX,2
	JLE	L70
	CMP	EBX,20
	JG	L70
	MOV	EAX,DWORD [-1072+EBP]
	MOV	EDX,DWORD [-1076+EBP]
	MOV	DWORD [-1132+EBP],EAX
	MOV	ECX,DWORD [12+ECX]
	MOV	EAX,DWORD [-1144+EBP]
	MOV	DWORD [-1136+EBP],EDX
	MOV	DWORD [-1140+EBP],ECX
	MOV	EAX,DWORD [16+EAX]
	MOV	DWORD [-1096+EBP],EAX
L70:
	MOV	ECX,DWORD [-1144+EBP]
	MOV	EDX,DWORD [4+ECX]
	LEA	EAX,DWORD [-21+EDX]
	CMP	EAX,ESI
	JG	L77
	LEA	EAX,DWORD [-5+EDX]
	CMP	ESI,EAX
	JGE	L77
	CMP	EBX,4
	JLE	L77
	CMP	EBX,18
	JG	L77
	TEST	BYTE [28+ECX],16
	JE	L77
	MOV	EBX,DWORD [36+ECX]
	PUSH	LC2
L89:
	PUSH	DWORD [148+EBX]
	CALL	_cons_putstr0
	CALL	_io_cli
	LEA	EAX,DWORD [48+EBX]
	MOV	DWORD [84+EBX],EAX
	MOV	DWORD [76+EBX],_asm_end_app
	CALL	_io_sti
L82:
	POP	EAX
	POP	EDX
	JMP	L77
L94:
	MOV	DWORD [-1076+EBP],0
	JMP	L58
L93:
	MOV	DWORD [-1072+EBP],0
	JMP	L57
L92:
	CMP	DWORD [-1080+EBP],383
	JG	L19
	CMP	DWORD [-1120+EBP],0
	JE	L20
	MOV	ECX,DWORD [-1080+EBP]
	MOV	AL,BYTE [_keytable1.1-256+ECX]
L87:
	MOV	BYTE [-124+EBP],AL
L22:
	MOV	DL,BYTE [-124+EBP]
	LEA	EAX,DWORD [-65+EDX]
	CMP	AL,25
	JA	L23
	TEST	DWORD [-1124+EBP],4
	JNE	L81
	CMP	DWORD [-1120+EBP],0
	JE	L25
L23:
	MOV	AL,BYTE [-124+EBP]
	TEST	AL,AL
	JNE	L97
	CMP	DWORD [-1080+EBP],271
	JE	L98
	CMP	DWORD [-1080+EBP],298
	JE	L99
	CMP	DWORD [-1080+EBP],310
	JE	L100
	CMP	DWORD [-1080+EBP],426
	JE	L101
	CMP	DWORD [-1080+EBP],438
	JE	L102
	CMP	DWORD [-1080+EBP],314
	JE	L103
	CMP	DWORD [-1080+EBP],325
	JE	L104
	CMP	DWORD [-1080+EBP],326
	JE	L105
	CMP	DWORD [-1080+EBP],506
	JE	L106
	CMP	DWORD [-1080+EBP],510
	JE	L107
	CMP	DWORD [-1080+EBP],315
	JNE	L50
	CMP	DWORD [-1120+EBP],0
	JNE	L108
L50:
	CMP	DWORD [-1080+EBP],343
	JNE	L77
	MOV	ECX,DWORD [-1104+EBP]
	MOV	EAX,DWORD [16+ECX]
	CMP	EAX,2
	JLE	L77
	DEC	EAX
	PUSH	EAX
	PUSH	DWORD [24+ECX]
	CALL	_sheet_updown
	JMP	L82
L108:
	MOV	EBX,DWORD [36+EDI]
	TEST	EBX,EBX
	JE	L77
	CMP	DWORD [52+EBX],0
	JE	L77
	PUSH	LC1
	JMP	L89
L107:
	CALL	_wait_KBC_sendready
	PUSH	DWORD [-1128+EBP]
	PUSH	96
	CALL	_io_out8
	JMP	L82
L106:
	MOV	DWORD [-1128+EBP],-1
	JMP	L77
L105:
	XOR	DWORD [-1124+EBP],1
L83:
	PUSH	237
	LEA	EBX,DWORD [-76+EBP]
	PUSH	EBX
	CALL	_fifo32_put
	PUSH	DWORD [-1124+EBP]
	PUSH	EBX
	CALL	_fifo32_put
	ADD	ESP,16
	JMP	L77
L104:
	XOR	DWORD [-1124+EBP],2
	JMP	L83
L103:
	XOR	DWORD [-1124+EBP],4
	JMP	L83
L102:
	AND	DWORD [-1120+EBP],-3
	JMP	L77
L101:
	AND	DWORD [-1120+EBP],-2
	JMP	L77
L100:
	OR	DWORD [-1120+EBP],2
	JMP	L77
L99:
	OR	DWORD [-1120+EBP],1
	JMP	L77
L98:
	PUSH	EDI
	CALL	_keywin_off
	MOV	ECX,DWORD [24+EDI]
	DEC	ECX
	POP	EAX
	JNE	L30
	MOV	EAX,DWORD [-1104+EBP]
	MOV	ECX,DWORD [16+EAX]
	DEC	ECX
L30:
	MOV	EDX,DWORD [-1104+EBP]
	MOV	EDI,DWORD [20+EDX+ECX*4]
	PUSH	EDI
	CALL	_keywin_on
L84:
	POP	ESI
	JMP	L77
L97:
	MOVSX	EAX,AL
	ADD	EAX,256
	PUSH	EAX
	MOV	EAX,DWORD [36+EDI]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	JMP	L82
L25:
	LEA	EAX,DWORD [32+EDX]
	MOV	BYTE [-124+EBP],AL
	JMP	L23
L81:
	CMP	DWORD [-1120+EBP],0
	JE	L23
	JMP	L25
L20:
	MOV	EDX,DWORD [-1080+EBP]
	MOV	AL,BYTE [_keytable0.0-256+EDX]
	JMP	L87
L19:
	MOV	BYTE [-124+EBP],0
	JMP	L22
L91:
	CMP	DWORD [-1084+EBP],0
	JNS	L109
	CMP	DWORD [-1092+EBP],2147483647
	JNE	L110
	PUSH	DWORD [-1116+EBP]
	CALL	_task_sleep
	CALL	_io_sti
	JMP	L84
L110:
	CALL	_io_sti
	JMP	L85
L109:
	CALL	_io_sti
	PUSH	DWORD [-1088+EBP]
	PUSH	DWORD [-1084+EBP]
	PUSH	DWORD [-1112+EBP]
	CALL	_sheet_slide
	MOV	DWORD [-1084+EBP],-1
	JMP	L86
L90:
	PUSH	EBX
	CALL	_fifo32_get
	MOV	DWORD [-1128+EBP],EAX
	CALL	_wait_KBC_sendready
	PUSH	DWORD [-1128+EBP]
	PUSH	96
	CALL	_io_out8
	ADD	ESP,12
	JMP	L10
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
	JNE	L113
L111:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L113:
	PUSH	3
	MOV	EAX,DWORD [36+EBX]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	POP	EBX
	POP	EAX
	JMP	L111
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
	JNE	L116
L114:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L116:
	PUSH	2
	MOV	EAX,DWORD [36+EBX]
	ADD	EAX,16
	PUSH	EAX
	CALL	_fifo32_put
	POP	ECX
	POP	EBX
	JMP	L114
