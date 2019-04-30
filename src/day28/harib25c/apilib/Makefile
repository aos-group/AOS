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

OBJS_API		= api001.obj api002.obj api003.obj api004.obj api005.obj api006.obj api007.obj api008.obj api009.obj api010.obj api011.obj api012.obj api013.obj api014.obj api015.obj api016.obj api017.obj api018.obj api019.obj api020.obj api021.obj api022.obj api023.obj api024.obj \
				  alloca.obj

# 默认行为
default:
	$(MAKE) apilib.lib

# 文件生成规则
apilib.lib: $(OBJS_API)
	$(GOLIB) $^ out:$@

# 一般规则
%.obj: %.nas
	$(NASK) $^ $@ $*.lst

# 命令
clean:
	-$(DEL) *.lst
	-$(DEL) *.obj

clean_all:
	$(MAKE) clean
	-$(DEL) apilib.lib