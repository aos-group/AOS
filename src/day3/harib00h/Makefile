VPATH 			= ../../../z_tools
Z_TOOLS_PATH 	= ../../../z_tools
QEMU_PATH 		= ..\..\..\z_tools\qemu

MAKE 			= make.exe -r
NASK 			= nask.exe
EDIMG 			= edimg.exe
IMGTOL 			= imgtol.com
COPY 			= copy
DEL 			= del

# 默认行为

default:
	$(MAKE) img

# 文件生成规则

ipl10.bin: ipl10.nas
	$(NASK) $^ $@ ipl10.lst

haribote.sys: haribote.nas
	$(NASK) $^ $@ haribote.lst

haribote.img: ipl10.bin haribote.sys
	$(EDIMG) imgin:$(Z_TOOLS_PATH)/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		imgout:haribote.img

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
	-$(DEL) ipl10.bin
	-$(DEL) ipl10.lst
	-$(DEL) haribote.sys
	-$(DEL) haribote.lst
	-$(DEL) $(QEMU_PATH)\fdimage0.bin

clean_all:
	$(MAKE) clean
	-$(DEL) haribote.img