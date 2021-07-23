
cc = g++ -g -m32

flag= -I./

TARGET = ResAccess.dll

SRC = ResAccess.cpp

all:
	@$(cc) $(flag) -fPIC -shared -o $(TARGET) $(SRC)


clean:
	@rm -rf *.o $(TARGET)

