echo ........................................................................... LiveFace

del LiveFace.exe

::set INCLUDE=%INCLUDE%;C:\Program Files\BCGSoft\BCGControlBarPro\BCGCBPro

::set LIB=%LIB%;C:\Program Files\BCGSoft\BCGControlBarPro\Bin

set CFG=LiveFace - Win32 Release

nmake -nologo LiveFace.mak

copy LiveFace.exe ..\LiveFacePack\FILE\LiveFace.exe

del LiveFace.exe
