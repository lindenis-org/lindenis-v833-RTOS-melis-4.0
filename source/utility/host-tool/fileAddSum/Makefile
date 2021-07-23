TARGET=FileAddSum
cc = gcc -static -g -Wall -m32
ccflags = -I./
STRIP=strip

all:
	$(cc) $(ccflags) -o FileAddSum addsum.c
	@$(STRIP) $(TARGET)
	@cp FileAddSum ../../../workspace/suniv/beetles/packtool/FileAddSum

clean:
	@rm -rf FileAddSum *.o
