Z_TOOLS_PATH 	= ../../../../z_tools
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
GOLIB			= golib00.exe

OBJS_BOOTPACK 	= bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj kbdms.obj memory.obj console.obj sheet.obj timer.obj mtask.obj file.obj window.obj

# 默认行为
default:
	$(MAKE) ipl20.bin
	$(MAKE) haribote.sys

# 文件生成规则
ipl20.bin: ipl20.nas
	$(NASK) $^ $@ ipl20.lst

asmhead.bin: asmhead.nas
	$(NASK) $^ $@ asmhead.lst

hankaku.bin: hankaku.txt
	$(MAKEFONT) $^ $@

hankaku.obj: hankaku.bin
	$(BIN2OBJ) $^ $@ _hankaku

bootpack.bim: $(OBJS_BOOTPACK)
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:3136k map:bootpack.map $^
#3MB + 64KB = 3136KB

%.hrb: %.bim
	$(BIM2HRB) $^ $@ 0

haribote.sys: asmhead.bin bootpack.hrb
	copy /B $<+bootpack.hrb $@

# 一般规则
%.gas: %.c
	$(CC1) -o $@ $^

%.nas: %.gas
	$(GAS2NASK) $^ $@

%.obj: %.nas
	$(NASK) $^ $@ $*.lst

# 命令
clean:
	-$(DEL) asmhead.bin
	-$(DEL) hankaku.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.hrb
	-$(DEL) *.map
	-$(DEL) *.bim

clean_all:
	$(MAKE) clean
	-$(DEL) ipl20.bin
	-$(DEL) haribote.sys