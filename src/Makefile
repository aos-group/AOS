TOOLS			= ../tools
ENTRY_POINT 	= 0xc0001500
FLOPPY_DIR		= path-to-empty-directory

MAKE 			= make -r
COPY 			= cp
DEL 			= rm
QEMU 			= qemu

# 默认行为
# 生成一个包含操作系统内核及全部应用的磁盘映像
default:
	$(MAKE) haribote.img

# 文件生成规则
haribote.img : haribote/ipl.bin haribote/haribote.sys \
		a/a.hrb hello1/hello1.hrb \
		winhelo/winhelo.hrb \
		stars/stars.hrb \
		lines/lines.hrb walk/walk.hrb noodle/noodle.hrb \
		beepdown/beepdown.hrb beepup/beepup.hrb color/color.hrb color2/color2.hrb \
		sosu/sosu.hrb type/type.hrb iroha/iroha.hrb \
		chklang/chklang.hrb \
		notrec/notrec.hrb bball/bball.hrb invader/invader.hrb calc/calc.hrb \
		tview/tview.hrb mmlplay/mmlplay.hrb gview/gview.hrb
	dd if=haribote/ipl.bin of=$@ bs=512 count=1
	dd if=/dev/zero of=$@ bs=512 seek=1 count=2879
	mkdir -p $(FLOPPY_DIR)
	sudo mount -o loop $@ $(FLOPPY_DIR) -o fat=12		#  -o loop: 用来把一个文件当成硬盘分区挂接上系统	-o fat=12: 挂载fat12文件系统
	#sleep 1
	sudo $(COPY) haribote/haribote.sys $(FLOPPY_DIR)
	sudo $(COPY) haribote/ipl.s $(FLOPPY_DIR)
	sudo $(COPY) a/a.hrb $(FLOPPY_DIR)
	sudo $(COPY) hello1/hello1.hrb $(FLOPPY_DIR)
	sudo $(COPY) winhelo/winhelo.hrb $(FLOPPY_DIR)
	sudo $(COPY) stars/stars.hrb $(FLOPPY_DIR)
	sudo $(COPY) lines/lines.hrb $(FLOPPY_DIR)
	sudo $(COPY) walk/walk.hrb $(FLOPPY_DIR)
	sudo $(COPY) noodle/noodle.hrb $(FLOPPY_DIR)
	sudo $(COPY) beepdown/beepdown.hrb $(FLOPPY_DIR)
	sudo $(COPY) beepup/beepup.hrb $(FLOPPY_DIR)
	sudo $(COPY) color/color.hrb $(FLOPPY_DIR)
	sudo $(COPY) color2/color2.hrb $(FLOPPY_DIR)
	sudo $(COPY) sosu/sosu.hrb $(FLOPPY_DIR)
	sudo $(COPY) type/type.hrb $(FLOPPY_DIR)
	sudo $(COPY) iroha/iroha.hrb $(FLOPPY_DIR)
	sudo $(COPY) chklang/chklang.hrb $(FLOPPY_DIR)
	sudo $(COPY) langdata/nihongo.nas $(FLOPPY_DIR)
	sudo $(COPY) langdata/chinese.txt $(FLOPPY_DIR)
	sudo $(COPY) langdata/chinese.nas $(FLOPPY_DIR)
	sudo $(COPY) notrec/notrec.hrb $(FLOPPY_DIR)
	sudo $(COPY) bball/bball.hrb $(FLOPPY_DIR)
	sudo $(COPY) invader/invader.hrb $(FLOPPY_DIR)
	sudo $(COPY) calc/calc.hrb $(FLOPPY_DIR)
	sudo $(COPY) tview/tview.hrb $(FLOPPY_DIR)
	sudo $(COPY) mmlplay/mmlplay.hrb $(FLOPPY_DIR)
	sudo $(COPY) mmldata/kirakira.mml $(FLOPPY_DIR)
	sudo $(COPY) mmldata/fujisan.mml $(FLOPPY_DIR)
	sudo $(COPY) mmldata/daigo.mml $(FLOPPY_DIR)
	sudo $(COPY) mmldata/daiku.mml $(FLOPPY_DIR)
	sudo $(COPY) gview/gview.hrb $(FLOPPY_DIR)
	sudo $(COPY) pictdata/fujisan.jpg $(FLOPPY_DIR)
	sudo $(COPY) pictdata/night.bmp $(FLOPPY_DIR)
	sudo $(COPY) fontlibrary/nihongo.fnt $(FLOPPY_DIR)
	sudo $(COPY) fontlibrary/chinese.fnt $(FLOPPY_DIR)
	#sleep 1
	sudo umount $(FLOPPY_DIR)
	
# 命令
# "make" 后启动 QEMU
run:
	$(MAKE) haribote.img
	$(QEMU) -fda haribote.img -m 32

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
	$(MAKE) -C notrec
	$(MAKE) -C bball
	$(MAKE) -C invader
	$(MAKE) -C calc
	$(MAKE) -C tview
	$(MAKE) -C mmlplay
	$(MAKE) -C gview
	$(MAKE) haribote.img

# "make full" 后 "make run"
run_full:
	$(MAKE) full
	$(QEMU) -drive file=haribote.img,if=floppy

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
	$(MAKE) -C notrec 		clean
	$(MAKE) -C bball 		clean
	$(MAKE) -C invader 		clean
	$(MAKE) -C calc 		clean
	$(MAKE) -C tview 		clean
	$(MAKE) -C mmlplay 		clean
	$(MAKE) -C gview 		clean

# 对操作系统核心、apilib 和应用程序全部执行 "make clean_all", 这样将清除所有的临时文件和最终生成物，不过执行这个命令后，"make" 和 "make run" 就无法使用了(用带 full 版本的命令代替即可), make 时会消耗更多的时间
clean_all_full:
	$(MAKE) -C haribote 	clean_all
	$(MAKE) -C apilib 		clean_all
	$(MAKE) -C a 			clean_all
	$(MAKE) -C hello1 		clean_all
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
	$(MAKE) -C notrec 		clean_all
	$(MAKE) -C bball 		clean_all
	$(MAKE) -C invader 		clean_all
	$(MAKE) -C calc 		clean_all
	$(MAKE) -C tview 		clean_all
	$(MAKE) -C mmlplay 		clean_all
	$(MAKE) -C gview 		clean_all
	-$(DEL) haribote.img

# "make full" 后 "make clean_full". 从执行过 "make clean_all_full" 的状态执行这个命令的话，就会恢复到可以直接 "make" 和 "make run" 的状态
refresh:
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) haribote.img
