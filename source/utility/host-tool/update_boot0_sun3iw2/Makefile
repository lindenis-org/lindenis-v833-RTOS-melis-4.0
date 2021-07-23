
CC:=gcc -static -g -Wall -m32 -o

STRIP=strip
TARGET=update_boot0_sun3iw2

all:$(TARGET)
$(TARGET):
	$(CC) $(TARGET) check.c script.c update_boot0.c -m32
	@$(STRIP) $(TARGET)
	cp  $(TARGET) ../../../workspace/suniv/beetles/packtool/$(TARGET)

.PHONY: clean
clean:
	rm -fr $(TARGET)
	@echo Build $(TARGET) Success!!
