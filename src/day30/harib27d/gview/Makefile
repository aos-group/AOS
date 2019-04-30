APP      = gview
STACK    = 4480k
MALLOC   = 0k

Z_TOOLS_PATH 	= ../../../../z_tools
INCPATH			= $(Z_TOOLS_PATH)/haribote
APILIBPATH   	= ../apilib/
HARIBOTEPATH 	= ../haribote/
FATHERPATH		= ../
QEMU_PATH 		= ..\..\..\z_tools\qemu
VPATH			= $(Z_TOOLS_PATH):$(APILIBPATH):$(HARIBOTEPATH):$(FATHERPATH)

MAKE 			= make.exe -r
NASK 			= nask.exe				# 将 nask 汇编编译为机器语言 (目标文件，未链接)
EDIMG 			= edimg.exe
IMGTOL 			= imgtol.com
COPY 			= copy
DEL 			= del
CC1				= cc1.exe -I $(INCPATH) -I ../ -Os -Wall -quiet	# 将 C 语言编译为 gas 汇编
GAS2NASK		= gas2nask.exe -a
OBJ2BIM			= obj2bim.exe			# 将目标文件链接为二进制映像文件
BIM2HRB			= bim2hrb.exe			# 针对操作系统修改为可以使用的机器语言文件
RULEFILE		= $(Z_TOOLS_PATH)/haribote/haribote.rul 	# 若要修改 Z_TOOLS_PATH，需要修改该文件内容中的路径！！！
MAKEFONT		= makefont.exe
BIN2OBJ			= bin2obj.exe
GOLIB			= golib00.exe
BIM2BIN			= bim2bin.exe

# 默认行为
default:
	$(MAKE) $(APP).hrb 

# 文件生成规则
$(APP).bim: $(APP).obj apilib.lib bmp.obj jpeg.obj
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:$(STACK) map:$(APP).map $^

$(APP).org: $(APP).bim
	$(BIM2HRB) $^ $@ $(MALLOC)

$(APP).hrb: $(APP).org
	$(BIM2BIN) -osacmp in:$^ out:$@

haribote.img: ipl.bin haribote.sys $(APP).hrb
	$(EDIMG) imgin:$(Z_TOOLS_PATH)/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		copy from:$(APP).hrb to:@: \
		imgout:haribote.img

# 一般规则
%.gas: %.c
	$(CC1) -o $@ $^

%.nas: %.gas
	$(GAS2NASK) $^ $@

%.obj: %.nas
	$(NASK) $^ $@ $*.lst

# 命令
run:
	$(MAKE) haribote.img
	$(COPY) haribote.img $(QEMU_PATH)\fdimage0.bin
	$(MAKE) -C $(Z_TOOLS_PATH)/qemu

full:
	$(MAKE) -C $(APILIBPATH)
	$(MAKE) $(APP).hrb

run_full:
	$(MAKE) -C $(APILIBPATH)
	$(MAKE) -C $(HARIBOTEPATH)
	$(MAKE) run

clean:
	-$(DEL) *.lst
	-$(DEL) gview.obj
	-$(DEL) jpeg.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.org
	-$(DEL) haribote.img

clean_all:
	$(MAKE) clean
	-$(DEL) $(APP).hrb
