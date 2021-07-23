cc = g++ -Wall -m32 -fshort-wchar -g -o
#cc = g++ -m32  -g -o

RES_PATH_F1C800=../../../projects/beetles/sun3iw2_app/res/lang/
RES_PATH_F1C200S=../../../projects/beetles/sun3iw1_app/res/lang/

#OBJ_TARGET = string.o
OBJ_TARGET = langOSDBuild
ccflag-s =

SRC = LangOsdBuild.cpp OsdVectorMan.cpp ParsResText.cpp Markup.cpp basefun.cpp
STRIP=strip

all:TARGET

TARGET:
	@$(cc) $(ccflag-s) $(OBJ_TARGET) $(SRC)
	@$(STRIP) $(OBJ_TARGET)
	@cp $(OBJ_TARGET) $(RES_PATH_F1C800)/$(OBJ_TARGET)
	@cp $(OBJ_TARGET) $(RES_PATH_F1C200S)/$(OBJ_TARGET)

clean:
	@rm -rf *.o $(OBJ_TARGET) $(RES_PATH_F1C200S)/$(OBJ_TARGET) $(RES_PATH_F1C800)/$(OBJ_TARGET)
