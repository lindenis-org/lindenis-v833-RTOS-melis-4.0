# Microsoft Developer Studio Generated NMAKE File, Based on LiveFace.dsp
!IF "$(CFG)" == ""
CFG=LiveFace - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LiveFace - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LiveFace - Win32 Release" && "$(CFG)" != "LiveFace - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LiveFace.mak" CFG="LiveFace - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LiveFace - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LiveFace - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LiveFace - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : ".\LiveFace.exe" "$(OUTDIR)\LiveFace.bsc"


CLEAN :
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\FaceBuilder.obj"
	-@erase "$(INTDIR)\FaceBuilder.sbr"
	-@erase "$(INTDIR)\FaceDocument.obj"
	-@erase "$(INTDIR)\FaceDocument.sbr"
	-@erase "$(INTDIR)\FaceDocumentBuilder.obj"
	-@erase "$(INTDIR)\FaceDocumentBuilder.sbr"
	-@erase "$(INTDIR)\FaceDocumentData.obj"
	-@erase "$(INTDIR)\FaceDocumentData.sbr"
	-@erase "$(INTDIR)\FaceResource.obj"
	-@erase "$(INTDIR)\FaceResource.sbr"
	-@erase "$(INTDIR)\FaceScript.obj"
	-@erase "$(INTDIR)\FaceScript.sbr"
	-@erase "$(INTDIR)\FaceScriptData.obj"
	-@erase "$(INTDIR)\FaceScriptData.sbr"
	-@erase "$(INTDIR)\LiveFace.obj"
	-@erase "$(INTDIR)\LiveFace.pch"
	-@erase "$(INTDIR)\LiveFace.res"
	-@erase "$(INTDIR)\LiveFace.sbr"
	-@erase "$(INTDIR)\LiveFaceDoc.obj"
	-@erase "$(INTDIR)\LiveFaceDoc.sbr"
	-@erase "$(INTDIR)\LiveFaceView.obj"
	-@erase "$(INTDIR)\LiveFaceView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\PlugInCenter.obj"
	-@erase "$(INTDIR)\PlugInCenter.sbr"
	-@erase "$(INTDIR)\PlugInWrapper.obj"
	-@erase "$(INTDIR)\PlugInWrapper.sbr"
	-@erase "$(INTDIR)\PreviewBar.obj"
	-@erase "$(INTDIR)\PreviewBar.sbr"
	-@erase "$(INTDIR)\scbarcf.obj"
	-@erase "$(INTDIR)\scbarcf.sbr"
	-@erase "$(INTDIR)\scbarg.obj"
	-@erase "$(INTDIR)\scbarg.sbr"
	-@erase "$(INTDIR)\SingleProcessWrapper.obj"
	-@erase "$(INTDIR)\SingleProcessWrapper.sbr"
	-@erase "$(INTDIR)\sizecbar.obj"
	-@erase "$(INTDIR)\sizecbar.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LiveFace.bsc"
	-@erase ".\LiveFace.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LiveFace.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\LiveFace.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LiveFace.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\FaceBuilder.sbr" \
	"$(INTDIR)\FaceDocument.sbr" \
	"$(INTDIR)\FaceDocumentBuilder.sbr" \
	"$(INTDIR)\FaceDocumentData.sbr" \
	"$(INTDIR)\FaceResource.sbr" \
	"$(INTDIR)\FaceScript.sbr" \
	"$(INTDIR)\FaceScriptData.sbr" \
	"$(INTDIR)\LiveFace.sbr" \
	"$(INTDIR)\LiveFaceDoc.sbr" \
	"$(INTDIR)\LiveFaceView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\PlugInCenter.sbr" \
	"$(INTDIR)\PlugInWrapper.sbr" \
	"$(INTDIR)\PreviewBar.sbr" \
	"$(INTDIR)\scbarcf.sbr" \
	"$(INTDIR)\scbarg.sbr" \
	"$(INTDIR)\sizecbar.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SingleProcessWrapper.sbr"

"$(OUTDIR)\LiveFace.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\LiveFace.pdb" /machine:I386 /out:"LiveFace.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\FaceBuilder.obj" \
	"$(INTDIR)\FaceDocument.obj" \
	"$(INTDIR)\FaceDocumentBuilder.obj" \
	"$(INTDIR)\FaceDocumentData.obj" \
	"$(INTDIR)\FaceResource.obj" \
	"$(INTDIR)\FaceScript.obj" \
	"$(INTDIR)\FaceScriptData.obj" \
	"$(INTDIR)\LiveFace.obj" \
	"$(INTDIR)\LiveFaceDoc.obj" \
	"$(INTDIR)\LiveFaceView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\PlugInCenter.obj" \
	"$(INTDIR)\PlugInWrapper.obj" \
	"$(INTDIR)\PreviewBar.obj" \
	"$(INTDIR)\scbarcf.obj" \
	"$(INTDIR)\scbarg.obj" \
	"$(INTDIR)\sizecbar.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LiveFace.res" \
	"$(INTDIR)\SingleProcessWrapper.obj"

".\LiveFace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LiveFace - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\LiveFace.exe" "$(OUTDIR)\LiveFace.bsc"


CLEAN :
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\FaceBuilder.obj"
	-@erase "$(INTDIR)\FaceBuilder.sbr"
	-@erase "$(INTDIR)\FaceDocument.obj"
	-@erase "$(INTDIR)\FaceDocument.sbr"
	-@erase "$(INTDIR)\FaceDocumentBuilder.obj"
	-@erase "$(INTDIR)\FaceDocumentBuilder.sbr"
	-@erase "$(INTDIR)\FaceDocumentData.obj"
	-@erase "$(INTDIR)\FaceDocumentData.sbr"
	-@erase "$(INTDIR)\FaceResource.obj"
	-@erase "$(INTDIR)\FaceResource.sbr"
	-@erase "$(INTDIR)\FaceScript.obj"
	-@erase "$(INTDIR)\FaceScript.sbr"
	-@erase "$(INTDIR)\FaceScriptData.obj"
	-@erase "$(INTDIR)\FaceScriptData.sbr"
	-@erase "$(INTDIR)\LiveFace.obj"
	-@erase "$(INTDIR)\LiveFace.pch"
	-@erase "$(INTDIR)\LiveFace.res"
	-@erase "$(INTDIR)\LiveFace.sbr"
	-@erase "$(INTDIR)\LiveFaceDoc.obj"
	-@erase "$(INTDIR)\LiveFaceDoc.sbr"
	-@erase "$(INTDIR)\LiveFaceView.obj"
	-@erase "$(INTDIR)\LiveFaceView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\PlugInCenter.obj"
	-@erase "$(INTDIR)\PlugInCenter.sbr"
	-@erase "$(INTDIR)\PlugInWrapper.obj"
	-@erase "$(INTDIR)\PlugInWrapper.sbr"
	-@erase "$(INTDIR)\PreviewBar.obj"
	-@erase "$(INTDIR)\PreviewBar.sbr"
	-@erase "$(INTDIR)\scbarcf.obj"
	-@erase "$(INTDIR)\scbarcf.sbr"
	-@erase "$(INTDIR)\scbarg.obj"
	-@erase "$(INTDIR)\scbarg.sbr"
	-@erase "$(INTDIR)\SingleProcessWrapper.obj"
	-@erase "$(INTDIR)\SingleProcessWrapper.sbr"
	-@erase "$(INTDIR)\sizecbar.obj"
	-@erase "$(INTDIR)\sizecbar.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LiveFace.bsc"
	-@erase "$(OUTDIR)\LiveFace.pdb"
	-@erase ".\LiveFace.exe"
	-@erase ".\LiveFace.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LiveFace.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /I /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\LiveFace.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LiveFace.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\FaceBuilder.sbr" \
	"$(INTDIR)\FaceDocument.sbr" \
	"$(INTDIR)\FaceDocumentBuilder.sbr" \
	"$(INTDIR)\FaceDocumentData.sbr" \
	"$(INTDIR)\FaceResource.sbr" \
	"$(INTDIR)\FaceScript.sbr" \
	"$(INTDIR)\FaceScriptData.sbr" \
	"$(INTDIR)\LiveFace.sbr" \
	"$(INTDIR)\LiveFaceDoc.sbr" \
	"$(INTDIR)\LiveFaceView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\PlugInCenter.sbr" \
	"$(INTDIR)\PlugInWrapper.sbr" \
	"$(INTDIR)\PreviewBar.sbr" \
	"$(INTDIR)\scbarcf.sbr" \
	"$(INTDIR)\scbarg.sbr" \
	"$(INTDIR)\sizecbar.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\SingleProcessWrapper.sbr"

"$(OUTDIR)\LiveFace.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\LiveFace.pdb" /debug /machine:I386 /out:"LiveFace.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\FaceBuilder.obj" \
	"$(INTDIR)\FaceDocument.obj" \
	"$(INTDIR)\FaceDocumentBuilder.obj" \
	"$(INTDIR)\FaceDocumentData.obj" \
	"$(INTDIR)\FaceResource.obj" \
	"$(INTDIR)\FaceScript.obj" \
	"$(INTDIR)\FaceScriptData.obj" \
	"$(INTDIR)\LiveFace.obj" \
	"$(INTDIR)\LiveFaceDoc.obj" \
	"$(INTDIR)\LiveFaceView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\PlugInCenter.obj" \
	"$(INTDIR)\PlugInWrapper.obj" \
	"$(INTDIR)\PreviewBar.obj" \
	"$(INTDIR)\scbarcf.obj" \
	"$(INTDIR)\scbarg.obj" \
	"$(INTDIR)\sizecbar.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LiveFace.res" \
	"$(INTDIR)\SingleProcessWrapper.obj"

".\LiveFace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LiveFace.dep")
!INCLUDE "LiveFace.dep"
!ELSE 
!MESSAGE Warning: cannot find "LiveFace.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LiveFace - Win32 Release" || "$(CFG)" == "LiveFace - Win32 Debug"
SOURCE=.\ChildFrm.cpp

"$(INTDIR)\ChildFrm.obj"	"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=..\LiveFaceCore\FaceBuilder.cpp

"$(INTDIR)\FaceBuilder.obj"	"$(INTDIR)\FaceBuilder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceDocument.cpp

"$(INTDIR)\FaceDocument.obj"	"$(INTDIR)\FaceDocument.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceDocumentBuilder.cpp

"$(INTDIR)\FaceDocumentBuilder.obj"	"$(INTDIR)\FaceDocumentBuilder.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceDocumentData.cpp

"$(INTDIR)\FaceDocumentData.obj"	"$(INTDIR)\FaceDocumentData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceResource.cpp

"$(INTDIR)\FaceResource.obj"	"$(INTDIR)\FaceResource.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceScript.cpp

"$(INTDIR)\FaceScript.obj"	"$(INTDIR)\FaceScript.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LiveFaceCore\FaceScriptData.cpp

"$(INTDIR)\FaceScriptData.obj"	"$(INTDIR)\FaceScriptData.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\LiveFace.cpp

"$(INTDIR)\LiveFace.obj"	"$(INTDIR)\LiveFace.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=.\LiveFace.rc

"$(INTDIR)\LiveFace.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\LiveFaceDoc.cpp

"$(INTDIR)\LiveFaceDoc.obj"	"$(INTDIR)\LiveFaceDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=.\LiveFaceView.cpp

"$(INTDIR)\LiveFaceView.obj"	"$(INTDIR)\LiveFaceView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=..\PlugInCenter\PlugInCenter.cpp

"$(INTDIR)\PlugInCenter.obj"	"$(INTDIR)\PlugInCenter.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\PlugInWrapper\PlugInWrapper.cpp

"$(INTDIR)\PlugInWrapper.obj"	"$(INTDIR)\PlugInWrapper.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\PreviewBar.cpp

"$(INTDIR)\PreviewBar.obj"	"$(INTDIR)\PreviewBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"


SOURCE=.\control_bar\scbarcf.cpp

"$(INTDIR)\scbarcf.obj"	"$(INTDIR)\scbarcf.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\control_bar\scbarg.cpp

"$(INTDIR)\scbarg.obj"	"$(INTDIR)\scbarg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\SingleProcessWrapper\SingleProcessWrapper.cpp

"$(INTDIR)\SingleProcessWrapper.obj"	"$(INTDIR)\SingleProcessWrapper.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\control_bar\sizecbar.cpp

"$(INTDIR)\sizecbar.obj"	"$(INTDIR)\sizecbar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LiveFace.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "LiveFace - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LiveFace.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\LiveFace.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "LiveFace - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LiveFace.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /I /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\LiveFace.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

