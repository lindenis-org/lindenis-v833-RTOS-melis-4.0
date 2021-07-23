
cc = gcc -static -g -Wall -m32
ccflag-s = -L./fstool -lfstool

src = main.c script_parser.c config.c config_private.c inifile.c build.c fsprocess_inter.c filepart.c ram_part.c

STRIP=strip

all:clean target

OUT_TARGET = fsbuild

target:
	@make -C fstool
	@$(cc) -o $(OUT_TARGET) $(src) $(SRC_FSTOOL) $(ccflag-s)
	@$(STRIP) $(OUT_TARGET)
	@cp $(OUT_TARGET) ../../../workspace/suniv/beetles/packtool/$(OUT_TARGET)

clean:
	@rm -rf *.o $(OUT_TARGET)
	@make -C fstool clean
