cc = g++  -Wall -m32 -fshort-wchar -g -o

RES_PATH_F1C800=./../../../projects/beetles/sun3iw2_app/res/theme/
REST_PATH_F1C200S=./../../../projects/beetles/sun3iw1_app/res/theme/

STRIP=strip
OBJ_TARGET = MakeScript
ccflag-s =

SRC = MakeScriptMain.cpp MakeScriptCore.cpp SynScript.cpp GenScript.cpp error.cpp inifile.cpp
CONFIG_SRC = config.cpp config_private.cpp

all:TARGET

TARGET:
	@$(cc) $(ccflag-s) $(OBJ_TARGET) $(SRC) $(CONFIG_SRC)
	@$(STRIP) $(OBJ_TARGET)
	@cp $(OBJ_TARGET) $(RES_PATH_F1C800)/$(OBJ_TARGET)
	@cp $(OBJ_TARGET) $(REST_PATH_F1C200S)/$(OBJ_TARGET)

clean:
	@rm -rf *.o $(OBJ_TARGET)
