//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//    I BBMMBB      BBMMBBI     BBI EEMMFFI BBMMI     BBI EEMMFFI BBMMI       BBMMBBI   EEMMI EEMMLL        //
//    EEI   EEFF  EEI   LLBB  EEMMBBI I BBEE  I EE  EEMMBBI I BBEE  I EE    EEI   LLBB    EEBBI I BB        //
//  I FF        I EE      BBI   EEI     EEI   I EE    EEI     EEI   I EE  I EE      BBI   EEI   I EE        //
//  LLFF        LLEE      EELL  EEI     EEI   I EE    EEI     EEI   I EE  LLEE      EELL  EEI   I EE        //
//  I EE      I I BB      EEI   EEI     EEI   I EE    EEI     EEI   I EE  I BB      EEI   EEI   I EE        //
//    BBLL  I I   BBFF  I EE    EEI     EEI   I BB    EEI     EEI   I BB    BBFF  I EE    BBI   I EE        //
//    I BBMMEE    I BBMMBB    EEMMMMLLBBMMBBLLMMMMEEEEMMMMLLBBMMBBLLMMMMEE  I BBMMBB    EEMMMMLLBBMMBB      //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                Scott System                                              //
//                                                                                                          //
//                               (c) Copyright 2006-2007, Scottyu China                                     //
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
//                                                                                                          //
// File    : error.cpp                                                                                      //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2008-04-14 13:32:00                                                                            //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2008-04-14 13:32:00                                                                                    //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//


#ifndef __ERROR__C__
#define __ERROR__C__    1

#include <stdarg.h>
#include "error.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>

using namespace std;

#ifndef SCOTT_DEBUG
#define SCOTT_DEBUG
#endif
#ifdef USE_COLOR_CONSOLE
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
//#include "consolescreen.h"
//CConsole console;
static int isconsoleinit = false;

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
static void initconsole()
{
    int i = 0;
    isconsoleinit = true;
    console.Create(FALSE);
    console.SetBkColor(0x88);
    console.SetTitle("debug system");
    console.SetConsoleMode(ENABLE_ECHO_INPUT |  ENABLE_LINE_INPUT  | ENABLE_PROCESSED_INPUT);
    console.ShowCursor(TRUE);
    console.SetTextAttribute(FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE);
}
#endif //USE_COLOR_CONSOLE


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
void Msg(const char *fmt, ...)
{
#ifdef USE_COLOR_CONSOLE
    if (!isconsoleinit)
    {
        initconsole();
    }
#endif //USE_COLOR_CONSOLE

    char format[MSGLEN];
    int cnt = 0;
    memset(format, 0, MSGLEN);

    va_list argptr;
    va_start(argptr, fmt);
    cnt = vsprintf(format, fmt, argptr);
    va_end(argptr);

#if (ERROR_OUTPUT == CONSOLE_OUTPUT_ERROR)

#ifdef USE_COLOR_CONSOLE
    console.SetTextColor(GREEN);
    console.Write(format);
#else
    printf("%s", format);
    //::OutputDebugString(format);
#endif //USE_COLOR_CONSOLE

#else
    MessageBox(NULL, format, "Message", MB_OK);
    //::OutputDebugString(format);
#endif
}


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
#ifdef SCOTT_DEBUG
void Warn(const char *fun, const char *file, int line, const  char *fmt, ...)
{
#ifdef USE_COLOR_CONSOLE
    if (!isconsoleinit)
    {
        initconsole();
    }
#endif //USE_COLOR_CONSOLE
    char szErr[MSGLEN * 2];
    char format[MSGLEN];
    int cnt = 0;
    memset(format, 0, MSGLEN);
    memset(szErr, 0, MSGLEN * 2);

    va_list argptr;
    va_start(argptr, fmt);
    cnt = vsprintf(format, fmt, argptr);
    va_end(argptr);


    sprintf(szErr, "WARN:%s[%d]\nFUNC:%s:%s\n", file, line, fun, format);
#if (ERROR_OUTPUT == CONSOLE_OUTPUT_ERROR)

#ifdef USE_COLOR_CONSOLE
    console.SetTextColor(YELLOW);
    console.Write(szErr);
#else
    printf("%s", szErr);
    //::OutputDebugString(szErr);
#endif //USE_COLOR_CONSOLE

#else
    MessageBox(NULL, szErr, "警告!", MB_OK);
#endif
}

#else //SOFT_RELEASE
//
#endif //


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
#ifdef SCOTT_DEBUG
void Err(const char *fun, const char *file, int line, const char *fmt, ...)
{
#ifdef USE_COLOR_CONSOLE
    if (!isconsoleinit)
    {
        initconsole();
    }
#endif //USE_COLOR_CONSOLE

    char szErr[MSGLEN * 2];
    char format[MSGLEN];
    int cnt = 0;
    memset(format, 0, MSGLEN);
    memset(szErr, 0, MSGLEN * 2);

    va_list argptr;
    va_start(argptr, fmt);
    cnt = vsprintf(format, fmt, argptr);
    va_end(argptr);

    sprintf(szErr, "ERROR:%s[%d]\nFUNC:%s:%s\n", file, line, fun, format);


#if (ERROR_OUTPUT == CONSOLE_OUTPUT_ERROR)

#ifdef USE_COLOR_CONSOLE
    console.SetTextColor(RED);
    console.Write(szErr);
#else
    printf("%s", szErr);
    //std::OutputDebugString(szErr);
#endif //USE_COLOR_CONSOLE


#else
    MessageBox(NULL, szErr, "出错啦!", MB_OK);
#endif
}

#else //SOFT_RELEASE
/*
void Err(char * fun, char * file, int line, char *fmt, ...)
{
    #ifdef USE_COLOR_CONSOLE
    if (!isconsoleinit)
    {
        initconsole();
    }
#endif //USE_COLOR_CONSOLE

    char szErr[MSGLEN*2];
    char format[MSGLEN];
    int cnt = 0;
    memset(format, 0, MSGLEN);
    memset(szErr, 0, MSGLEN*2);

    va_list argptr;
    va_start(argptr, fmt);
    cnt = vsprintf(format, fmt, argptr);
    va_end(argptr);

    sprintf(szErr, "ERROR:%s\n", format);
#if (ERROR_OUTPUT == CONSOLE_OUTPUT_ERROR)

#ifdef USE_COLOR_CONSOLE
    console.SetTextColor(RED);
    console.Write(szErr);
#else
    printf("%s", szErr);
#endif //USE_COLOR_CONSOLE


#else
    MessageBox(NULL,szErr, "出错啦!", MB_OK);
#endif
}
*/
#endif //


//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
#ifdef SCOTT_DEBUG
void Dbg(char *fun, char *file, int line, char *fmt, ...)
{
#ifdef USE_COLOR_CONSOLE
    if (!isconsoleinit)
    {
        initconsole();
    }
#endif //USE_COLOR_CONSOLE

    char szErr[MSGLEN * 2];
    char format[MSGLEN];
    int cnt = 0;
    memset(format, 0, MSGLEN);
    memset(szErr, 0, MSGLEN * 2);

    va_list argptr;
    va_start(argptr, fmt);
    cnt = vsprintf(format, fmt, argptr);
    va_end(argptr);

    sprintf(szErr, "DEBUG:%s[%d]\nFUNC:%s:%s\n", file, line, fun, format);
#if (ERROR_OUTPUT == CONSOLE_OUTPUT_ERROR)
#ifdef USE_COLOR_CONSOLE
    console.SetTextColor(RED);
    console.Write(szErr);
#else
    printf("%s", szErr);
#endif //USE_COLOR_CONSOLE

#else
    MessageBox(NULL, szErr, "调试!", MB_OK);
#endif
}

#else //SOFT_RELEASE
#endif //




//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
#ifdef SCOTT_DEBUG
void SPRINTF(char *szErr, char *fmt, ...)
{
    char s[400];
    int cnt;
    va_list argptr;

    va_start(argptr, fmt);
    cnt = vsprintf(s, fmt, argptr);
    va_end(argptr);

    strcpy(szErr, s);
}
#else //SOFT_RELEASE

#endif //



//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__ERROR__C__

