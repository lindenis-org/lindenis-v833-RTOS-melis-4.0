@echo off
goto START
/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              ePos Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : epos
* File    : makefile.bat
*
* By      : Sunny
* Version : v1.0
* Date    : 2008-9-8 14:23:22
**********************************************************************************************************************
*/
:ERROR
    pause
    goto EXIT

:START
    @echo =========================
    @echo = download fsys start
    @echo =========================
    
    if "%1"=="/b" goto build
    call D:\Winners\ePDK\softwares\MakeTool\setpath
    call setchip
::**************************************************************************
:: select via files
::**************************************************************************
:build
    call via D:\winners\ePDK\eMod\sys_fsys\download_fs

::**************************************************************************
:: compile and link
::**************************************************************************
    if exist tmplib.o del tmplib.o
    if exist tmplib.a del tmplib.a
    
    call cc test_main
	::call cc test_ram_part
    call makefile

    @echo Linking...
    armlink --noremove --via=%lvia% tmplib.o

    @if errorlevel 2 pause
    @if errorlevel 1 pause
    
    @echo Fromelf...
    fromelf --via=%fvia% __image.axf

    if exist tmplib.o del tmplib.o
    if exist tmplib.a del tmplib.a
::  if exist __image.axf del __image.axf

    @echo Finished!
    if not "%1"=="/b" pause
    
	exit