Z_TOOLS_PATH 	= ../../../z_tools
INCPATH			= $(Z_TOOLS_PATH)/haribote
QEMU_PATH 		= ..\..\..\z_tools\qemu
VPATH			= $(Z_TOOLS_PATH)

MAKE 			= make.exe -r
EDIMG 			= edimg.exe
IMGTOL 			= imgtol.com
COPY 			= copy
DEL 			= del

# 默认行为
# 生成一个包含操作系统内核及全部应用的磁盘映像
default:
	$(MAKE) haribote.img

# 文件生成规则
haribote.img : haribote/ipl.bin haribote/haribote.sys \
		a/a.hrb hello1/hello1.hrb hello2/hello2.hrb \
		winhelo/winhelo.hrb \
		stars/stars.hrb \
		lines/lines.hrb walk/walk.hrb noodle/noodle.hrb \
		beepdown/beepdown.hrb beepup/beepup.hrb color/color.hrb color2/color2.hrb \
		sosu/sosu.hrb type/type.hrb iroha/iroha.hrb \
		fontlibrary/nihongo.fnt chklang/chklang.hrb langtest.nas test.txt \
		fontlibrary/chinese.fnt
	$(EDIMG)   imgin:$(Z_TOOLS_PATH)/fdimg0at.tek \
		wbinimg src:haribote/ipl.bin len:512 from:0 to:0 \
		copy from:haribote/haribote.sys to:@: \
		copy from:haribote/ipl.nas to:@: \
		copy from:make.bat to:@: \
		copy from:a/a.hrb to:@: \
		copy from:hello1/hello1.hrb to:@: \
		copy from:hello2/hello2.hrb to:@: \
		copy from:winhelo/winhelo.hrb to:@: \
		copy from:stars/stars.hrb to:@: \
		copy from:lines/lines.hrb to:@: \
		copy from:walk/walk.hrb to:@: \
		copy from:noodle/noodle.hrb to:@: \
		copy from:beepdown/beepdown.hrb to:@: \
		copy from:beepup/beepup.hrb to:@: \
		copy from:color/color.hrb to:@: \
		copy from:color2/color2.hrb to:@: \
		copy from:sosu/sosu.hrb to:@: \
		copy from:type/type.hrb to:@: \
		copy from:iroha/iroha.hrb to:@: \
		copy from:chklang/chklang.hrb to:@: \
		copy from:langtest.nas to:@: \
		copy from:test.txt to:@: \
		copy from:fontlibrary/nihongo.fnt to:@: \
		copy from:fontlibrary/chinese.fnt to:@: \
		imgout:haribote.img

# 命令
# "make" 后启动 QEMU
run:
	$(MAKE) haribote.img
	$(COPY) haribote.img $(QEMU_PATH)\fdimage0.bin
	$(MAKE) -C $(Z_TOOLS_PATH)/qemu

# "make" 后将磁盘映像安装到软盘中
install:
	$(MAKE) haribote.img
	$(IMGTOL) w a: haribote.img

# 将操作系统核心、apilib 和应用程序全部 make 后生成磁盘映像
full:
	$(MAKE) -C haribote
	$(MAKE) -C apilib
	$(MAKE) -C a
	$(MAKE) -C hello1
	$(MAKE) -C hello2
	$(MAKE) -C winhelo
	$(MAKE) -C stars
	$(MAKE) -C lines
	$(MAKE) -C walk
	$(MAKE) -C noodle
	$(MAKE) -C beepup
	$(MAKE) -C beepdown
	$(MAKE) -C color
	$(MAKE) -C color2
	$(MAKE) -C sosu
	$(MAKE) -C iroha
	$(MAKE) -C chklang
	$(MAKE) haribote.img

# "make full" 后 "make run"
run_full:
	$(MAKE) full
	$(COPY) haribote.img $(QEMU_PATH)\fdimage0.bin
	$(MAKE) -C $(Z_TOOLS_PATH)/qemu

# "make full" 后 "make install"
install_full:
	$(MAKE) full
	$(IMGTOL) w a: haribote.img

# 将操作系统核心 make 后执行 "make run", 当只对操作系统核心进行修改时可使用这个命令
run_os:
	$(MAKE) -C haribote
	$(MAKE) run

# 本来 clean 命令是用于清除临时文件的，但由于在这个 Makefile 中并不生成临时文件，因此这个命令不执行任何操作
clean:
# 什么都不做

# 将生成的磁盘映像删除以释放磁盘空间
clean_all:
	$(MAKE) clean
	-$(DEL) haribote.img

# 对操作系统核心、apilib 和应用程序全部执行 "make clean", 这样将清除所有的临时文件
clean_full:
	$(MAKE) -C haribote 	clean
	$(MAKE) -C apilib 		clean
	$(MAKE) -C a 			clean
	$(MAKE) -C hello1 		clean
	$(MAKE) -C hello2 		clean
	$(MAKE) -C winhelo 		clean
	$(MAKE) -C stars 		clean
	$(MAKE) -C lines 		clean
	$(MAKE) -C walk 		clean
	$(MAKE) -C noodle 		clean
	$(MAKE) -C beepup  		clean
	$(MAKE) -C beepdown 	clean
	$(MAKE) -C color 		clean
	$(MAKE) -C color2 		clean
	$(MAKE) -C sosu 		clean
	$(MAKE) -C type 		clean
	$(MAKE) -C iroha 		clean
	$(MAKE) -C chklang 		clean

# 对操作系统核心、apilib 和应用程序全部执行 "make clean_all", 这样将清除所有的临时文件和最终生成物，不过执行这个命令后，"make" 和 "make run" 就无法使用了(用带 full 版本的命令代替即可), make 时会消耗更多的时间
clean_all_full:
	$(MAKE) -C haribote 	clean_all
	$(MAKE) -C apilib 		clean_all
	$(MAKE) -C a 			clean_all
	$(MAKE) -C hello1 		clean_all
	$(MAKE) -C hello2 		clean_all
	$(MAKE) -C winhelo 		clean_all
	$(MAKE) -C stars 		clean_all
	$(MAKE) -C lines 		clean_all
	$(MAKE) -C walk 		clean_all
	$(MAKE) -C noodle 		clean_all
	$(MAKE) -C beepup  		clean_all
	$(MAKE) -C beepdown 	clean_all
	$(MAKE) -C color 		clean_all
	$(MAKE) -C color2 		clean_all
	$(MAKE) -C sosu 		clean_all
	$(MAKE) -C type 		clean_all
	$(MAKE) -C iroha 		clean_all
	$(MAKE) -C chklang 		clean_all
	-$(DEL) haribote.img

# "make full" 后 "make clean_full". 从执行过 "make clean_all_full" 的状态执行这个命令的话，就会恢复到可以直接 "make" 和 "make run" 的状态
refresh:
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) haribote.img