echo off
@del *.ncb
@del *.plg
@del *.ilk
@del *.aps

@del debug\*.obj 
@del debug\*.sbr
@del debug\*.pch
@del debug\*.bsc
@del debug\*.idb
@del debug\*.ilk
@del debug\*.pdb
@del debug\*.exe
@del debug\*.exp
@del debug\*.lib
@del debug\*.res
@del debug < y

@del release\*.obj 
@del release\*.sbr
@del release\*.pch
@del release\*.bsc
@del release\*.idb
@del release\*.exe
@del release\*.exp
@del release\*.lib
@del release\*.res
@del release < y	

@rmdir debug
@rmdir release

@REM mkdir bkup
@REM copy *.rar bkup\*.rar
@REM del *.rar

echo on
