cc = gcc -static -finput-charset=UTF-8 -g -Wall
ccflag-s = -I./  -m32

lzma = lzma/LzFind.c lzma/LzmaDec.c lzma/LzmaEnc.c lzma/LzmaLib.c

src = minfs_main.c minfs_config.c config/config.c config/config_private.c inifile.c \
      minfs_module.c minfs_calc.c elf_parser.c minfs_compress.c \
      minfs_tool.c minfs_make.c minfs_dump.c minfs_util.c $(lzma)

TARGET=minfs
STRIP=strip

all:
	@$(cc) $(ccflag-s) -o minfs $(src)
	@$(STRIP) $(TARGET)
	@cp minfs ../../../workspace/suniv/beetles/packtool/minfs

clean:
	@rm -rf *.o minfs
