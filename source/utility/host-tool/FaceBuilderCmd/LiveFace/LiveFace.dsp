# Microsoft Developer Studio Project File - Name="LiveFace" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LiveFace - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LiveFace.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LiveFace - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"LiveFace.exe"

!ELSEIF  "$(CFG)" == "LiveFace - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /I /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"LiveFace.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LiveFace - Win32 Release"
# Name "LiveFace - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocumentBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocumentData.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceResource.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceScript.cpp
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceScriptData.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveFace.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveFace.rc
# End Source File
# Begin Source File

SOURCE=.\LiveFaceDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\LiveFaceView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\PlugInCenter\PlugInCenter.cpp
# End Source File
# Begin Source File

SOURCE=..\PlugInWrapper\PlugInWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\PreviewBar.cpp
# End Source File
# Begin Source File

SOURCE=.\control_bar\scbarcf.cpp
# End Source File
# Begin Source File

SOURCE=.\control_bar\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=..\SingleProcessWrapper\SingleProcessWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\control_bar\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceBuilder.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocument.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocumentBuilder.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceDocumentData.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceResource.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceScript.h
# End Source File
# Begin Source File

SOURCE=..\LiveFaceCore\FaceScriptData.h
# End Source File
# Begin Source File

SOURCE=.\LiveFace.h
# End Source File
# Begin Source File

SOURCE=.\LiveFaceDoc.h
# End Source File
# Begin Source File

SOURCE=.\LiveFaceView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\PlugInCenter\PlugInCenter.h
# End Source File
# Begin Source File

SOURCE=..\PlugInWrapper\PlugInWrapper.h
# End Source File
# Begin Source File

SOURCE=.\PreviewBar.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\control_bar\scbarcf.h
# End Source File
# Begin Source File

SOURCE=.\control_bar\scbarg.h
# End Source File
# Begin Source File

SOURCE=..\SingleProcessWrapper\SingleProcessWrapper.h
# End Source File
# Begin Source File

SOURCE=.\control_bar\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\LiveFace.ico
# End Source File
# Begin Source File

SOURCE=.\res\LiveFace.rc2
# End Source File
# Begin Source File

SOURCE=.\res\LiveFaceDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\LiveFace.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
