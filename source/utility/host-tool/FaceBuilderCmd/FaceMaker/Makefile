
cc = g++ -g -m32

flag= -I./

TARGET = facemaker.dll

SRC = FaceMaker.cpp error.cpp fun.cpp

all:
	@$(cc) $(flag) -fPIC -shared -o $(TARGET) $(SRC)


clean:
	@rm -rf *.o $(TARGET)

