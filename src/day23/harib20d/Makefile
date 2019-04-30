Z_TOOLS_PATH 	= ../../../z_tools
QEMU_PATH 		= ..\..\..\z_tools\qemu
INCPATH			= $(Z_TOOLS_PATH)/haribote
VPATH			= $(Z_TOOLS_PATH)

MAKE 			= make.exe -r
NASK 			= nask.exe				# 将 nask 汇编编译为机器语言 (目标文件，未链接)
EDIMG 			= edimg.exe
IMGTOL 			= imgtol.com
COPY 			= copy
DEL 			= del
CC1				= cc1.exe -I $(INCPATH) -Os -Wall -quiet	# 将 C 语言编译为 gas 汇编
GAS2NASK		= gas2nask.exe -a
OBJ2BIM			= obj2bim.exe			# 将目标文件链接为二进制映像文件
BIM2HRB			= bim2hrb.exe			# 针对操作系统修改为可以使用的机器语言文件
RULEFILE		= $(Z_TOOLS_PATH)/haribote/haribote.rul 	# 若要修改 Z_TOOLS_PATH，需要修改该文件内容中的路径！！！
MAKEFONT		= makefont.exe
BIN2OBJ			= bin2obj.exe

OBJS_BOOTPACK 	= bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj kbdms.obj memory.obj sheet.obj timer.obj mtask.obj file.obj window.obj console.obj

# 默认行为

default:
	$(MAKE) img

# 文件生成规则

hello.hrb: hello.nas
	$(NASK) $^ $@ $*.lst

ipl10.bin: ipl10.nas
	$(NASK) $^ $@ ipl10.lst

asmhead.bin: asmhead.nas
	$(NASK) $^ $@ asmhead.lst

hankaku.bin: hankaku.txt
	$(MAKEFONT) $^ $@

hankaku.obj: hankaku.bin
	$(BIN2OBJ) $^ $@ _hankaku

bootpack.bim: $(OBJS_BOOTPACK)
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:3136k map:bootpack.map $^
#3MB + 64KB = 3136KB

%.bim: %.obj a_nask.obj
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:1k map:$*.map $^

%.hrb: %.bim
	$(BIM2HRB) $^ $@ 0

haribote.sys: asmhead.bin bootpack.hrb
	copy /B $<+bootpack.hrb $@

haribote.img: ipl10.bin haribote.sys hello.hrb a.hrb hello1.hrb hello2.hrb winhelo.hrb stars.hrb lines.hrb
	$(EDIMG) imgin:$(Z_TOOLS_PATH)/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		copy from:ipl10.nas to:@: \
		copy from:make.bat to:@: \
		copy from:hello.hrb to:@: \
		copy from:a.hrb to:@: \
		copy from:hello1.hrb to:@: \
		copy from:hello2.hrb to:@: \
		copy from:winhelo.hrb to:@: \
		copy from:stars.hrb to:@: \
		copy from:lines.hrb to:@: \
		imgout:haribote.img

# 一般规则
%.gas: %.c
	$(CC1) -o $@ $^

%.nas: %.gas
	$(GAS2NASK) $^ $@

%.obj: %.nas
	$(NASK) $^ $@ $*.lst

# 命令

asm:
	$(MAKE) ipl10.bin

img:
	$(MAKE) haribote.img

run:
	$(MAKE) img
	$(COPY) haribote.img $(QEMU_PATH)\fdimage0.bin
	$(MAKE) -C $(Z_TOOLS_PATH)/qemu

install:
	$(MAKE) img
	$(IMGTOL) w a: helloos.img

clean:
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.hrb
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) haribote.sys
	-$(DEL) $(QEMU_PATH)\fdimage0.bin

clean_all:
	$(MAKE) clean
	-$(DEL) haribote.img