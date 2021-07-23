
cc = g++ -static -g -m32

flag= -I./ -I./include

STRIP=strip
TARGET = facebuildercmd

RES_PATH_F1C800=./../../../projects/beetles/sun3iw2_app/res/theme/
REST_PATH_F1C200S=./../../../projects/beetles/sun3iw1_app/res/theme/

CONFIG_SRC = config/config.cpp config/config_private.cpp config/inifile.cpp

SCRIPTPARSER_SRC =  ScriptParser/scriptparser.cpp


FACEMAKER_SRC = FaceMaker/FaceMaker.cpp FaceMaker/fun.cpp

ResAccess_SRC = ResAccess/ResAccess.cpp

LIVECORE_SRC =  LiveFaceCore/FaceDocument.cpp LiveFaceCore/FaceBuilder.cpp \
      LiveFaceCore/FaceScript.cpp LiveFaceCore/FaceResource.cpp LiveFaceCore/FaceDocumentBuilder.cpp \
      LiveFaceCore/FaceScriptData.cpp LiveFaceCore/FaceDocumentData.cpp

SRC = FaceBuilderCmd.cpp FaceBuilderCore.cpp BCString.cpp error.cpp PlugInWrapper/PlugInWrapper.cpp \
      PlugInCenter/PlugInCenter.cpp \
      $(CONFIG_SRC) $(SCRIPTPARSER_SRC) \
      $(FACEMAKER_SRC) $(ResAccess_SRC) $(LIVECORE_SRC)

all:
	@$(cc) $(flag) -o $(TARGET) $(SRC) -ldl -lpthread
	@$(STRIP) $(TARGET)
	@cp $(TARGET) ./test/$(TARGET)
	@cp $(TARGET) $(RES_PATH_F1C800)/$(TARGET)
	@cp $(TARGET) $(RES_PATH_F1C200S)/$(TARGET)


clean:
	@rm -rf *.o $(TARGET)

