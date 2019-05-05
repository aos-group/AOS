TOOLS			= ../../tools
ENTRY_POINT 	= 0xc0001500
FLOPPY_DIR		= path-to-empty-directory

MAKE 			= make -r
AS 				= nasm
COPY 			= cp
DEL 			= rm
CC 				= gcc $(CFLAGS)
QEMU 			= qemu
BIM2HRB			= $(TOOLS)/bim2hrb
OBJ2BIM			= $(TOOLS)/obj2bim
RULEFILE		= $(TOOLS)/haribote.rul
C2NASM			= $(TOOLS)/c2nasm.sh
MAKEFONT		= $(TOOLS)/makefont
BIN2OBJ			= $(TOOLS)/bin2obj
CHSEC			= $(TOOLS)/chsec.sh

OBJS 	= bootpack.o naskfunc.o hankaku.o graphic.o dsctbl.o \
				  int.o fifo.o kbdms.o memory.o console.o sheet.o \
				  timer.o mtask.o file.o window.o tek.o

# 默认行为
default:
	$(MAKE) ipl.bin
	$(MAKE) haribote.sys

# 文件生成规则
ipl.bin: ipl.s
	$(AS) -o $@ $< -l ipl.lst

asmhead.bin: asmhead.s
	$(AS) -o $@ $< -l asmhead.lst

hankaku.bin: hankaku.txt
	$(MAKEFONT) $^ $@

hankaku.o: hankaku.bin
	$(BIN2OBJ) $^ $@ _hankaku

bootpack.bim: $(OBJS)
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:3136k map:bootpack.map $^
#3MB + 64KB = 3136KB

%.hrb: %.bim
	$(BIM2HRB) $^ $@ 0

haribote.sys: asmhead.bin bootpack.hrb
	cat $^ > $@

# 一般规则
.PRECIOUS: %.nas_tmp %.nas _hankaku

%.nas_tmp: %.c
	$(C2NASM) $< $@
	
%.nas: %.nas_tmp
	$(CHSEC) $< $@
	
%.o: %.s
	$(AS) -f coff -o $@ $< -l $*.lst

%.o: %.nas
	$(AS) -f coff -o $@ $< -l $*.lst

# 命令
clean:
	-$(DEL) asmhead.bin
	-$(DEL) hankaku.bin
	-$(DEL) *.lst
	-$(DEL) *.o
	-$(DEL) *.hrb
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.nas_tmp
	-$(DEL) *.nas

clean_all:
	$(MAKE) clean
	-$(DEL) ipl.bin
	-$(DEL) haribote.sys
