TOOLS			= ../../../tools
ENTRY_POINT 	= 0xc0001500
FLOPPY_DIR		= /tmp/floppy

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
GOLIB			= $(TOOLS)/golib00

OBJS_API		= api001.o api002.o api003.o api004.o api005.o \
				  api006.o api007.o api008.o api009.o api010.o \
				  api011.o api012.o api013.o api014.o api015.o \
				  api016.o api017.o api018.o api019.o api020.o \
				  api021.o api022.o api023.o api024.o api025.o \
				  api026.o api027.o \
				  alloca.o

# 默认行为
default:
	$(MAKE) apilib.lib

# 文件生成规则
apilib.lib: $(OBJS_API)
	$(GOLIB) $^ out:$@

# 一般规则
%.o: %.nas
	$(AS) -f coff -o $@ $< -l $*.lst

# 命令
clean:
	-$(DEL) *.lst
	-$(DEL) *.o

clean_all:
	$(MAKE) clean
	-$(DEL) apilib.lib
