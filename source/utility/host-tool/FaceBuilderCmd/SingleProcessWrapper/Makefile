
cc = g++ -g -m32

flag= -I./ -I../

TARGET = Single.dll

SRC = SingleProcessWrapper.cpp

all:
	@$(cc) $(flag) -fPIC -shared -o $(TARGET) $(SRC)


clean:
	@rm -rf *.o $(TARGET)

