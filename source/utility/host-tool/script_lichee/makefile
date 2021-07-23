
#from lichee

objects = script.o
TARGET=script
STRIP=strip

edit:$(objects)
	gcc -o script $(objects) -static
	@$(STRIP) $(TARGET)
	@cp script ../../../workspace/suniv/beetles/packtool/script


clean:
	rm -rf script $(objects)

