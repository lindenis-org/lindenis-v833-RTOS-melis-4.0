CC:=gcc -static -g -Wall -m32 -o

STRIP=strip
TARGET=update_uboot

all:$(TARGET)
$(TARGET):
	$(CC) $(TARGET) check.c script.c update_uboot.c -m32
	@$(STRIP) $(TARGET)
	cp  $(TARGET) ../../../workspace/suniv/beetles/packtool/$(TARGET)

.PHONY: clean
clean:
	rm -fr $(TARGET)
