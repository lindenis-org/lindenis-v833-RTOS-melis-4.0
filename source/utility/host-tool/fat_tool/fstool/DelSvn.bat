::******************************************************************************
::*                           Delete .svn tool
::* File        : DelSvn.bat
::*
::* Purpose     : Delete all ".svn" documnet under current diretory
::*
::* By          : Sunny                                       Date : 2009.11.8
::******************************************************************************
@echo off
echo ************************************
echo *    Delete .svn tool    	  		*
echo ************************************

:SELECT
echo Are you sure delete all .svn ?
echo ************************************
echo y: Yes
echo n: Cancel
echo ************************************
set /p SEL=Please Select:
if %SEL%==y     goto DELETE
if %SEL%==n     goto EXIT
@echo Selecti is invalid!!!
goto  SELECT

:DELETE
set DelNumber=0
for /d /r %%b in (.svn) do (
    if exist %%b (
        rd /s /q "%%b"
        set /a DelNumber+=1
    )
)
echo Total delete number : %DelNumber%

:EXIT
pause
