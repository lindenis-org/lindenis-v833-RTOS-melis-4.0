
cc = g++ -g -m32

flag= -I./

TARGET = config.dll

SRC = config.cpp config_private.cpp inifile.cpp

all:
	@$(cc) $(flag) -fPIC -shared -o $(TARGET) $(SRC)


clean:
	@rm -rf *.o $(TARGET)

