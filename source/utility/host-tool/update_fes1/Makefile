CC:=gcc -static -g -Wall -m32 -o

STRIP=strip
TARGET=update_fes1

all:$(TARGET)
$(TARGET):
	$(CC) $(TARGET) check.c script.c update_fes1.c -m32
	@$(STRIP) $(TARGET)
	@cp  $(TARGET) ../../../workspace/suniv/beetles/packtool/$(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
	@echo Build $(TARGET) Success!!
