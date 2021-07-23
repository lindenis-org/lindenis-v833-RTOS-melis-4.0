CC:=gcc -static -g -Wall -m32 -o

STRIP=strip
TARGET=update_mbr

all:$(TARGET)
$(TARGET):
	$(CC) $(TARGET) crc.c script.c update_mbr.c -m32
	@$(STRIP) $(TARGET)
	@cp  $(TARGET) ../../../workspace/suniv/beetles/packtool/$(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)

