TARGET=script
STRIP=strip
cc = gcc -static -g -Wall -m32

ccflag-s =

all:
	@$(cc) $(ccflag-s) -o script script.c
	@$(STRIP) $(TARGET)
	@cp script ../../../workspace/suniv/beetles/packtool/script

clean:
	@rm -rf *.o script
