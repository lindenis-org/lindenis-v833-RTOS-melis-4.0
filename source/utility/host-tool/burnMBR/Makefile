CC:=gcc -static -g -Wall -o

TARGET=BurnMBR
STRIP=strip

all:$(TARGET)

$(TARGET):
	@$(CC) $(TARGET) crc.c script.c BurnMBR.c -m32
	@$(STRIP) $(TARGET)
	@cp  $(TARGET) ../../../workspace/suniv/beetles/packtool/$(TARGET)

.PHONY: clean
clean:
	rm -fr $(TARGET)

