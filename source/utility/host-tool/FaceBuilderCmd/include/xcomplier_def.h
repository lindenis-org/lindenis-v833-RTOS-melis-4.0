#ifndef _X_COMPLIER_DEF_H__HEADER_
#define _X_COMPLIER_DEF_H__HEADER_
/*************************************************************/
/*define for Linux
**************************************************************/
#ifndef _WINDOWS
#include <pthread.h>
#include <dlfcn.h>
#include "basetypes.h"
//#include "PhoenixDebug.h"
//using namespace PhoenixDebug;
using namespace std;

#define DebugMsg printf
#define DebugErr printf

//------------------------------------------------//
// macro
//-----------------------------------------------//
#define MAX_PATH              260
#define INVALID_HANDLE_VALUE  (void *)-1
#define MB_OK                 0
#define IN
#define OUT
#define WM_QUIT                         0x0012
#define CP_ACP 0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*************************************************/
// varant
/*************************************************/
typedef const char    *LPCSTR, *PCSTR;
typedef const char    *LPCTSTR, *PCTSTR;
typedef char          *LPTSTR, *PTSTR;
typedef char          *LPSTR, *PSTR;
typedef bool           BOOL;
typedef void          *HMODULE;
typedef void          VOID, *HINSTANCE, *LPVOID, *PVOID;
typedef unsigned int   UINT;
typedef unsigned int   DWORD, *LPDWORD;
typedef DWORD *PDWORD;
typedef wchar_t        WCHAR;
typedef wchar_t    *LPWSTR;
typedef const wchar_t  *LPCWSTR;
typedef unsigned char *LPBYTE, BYTE;

#ifdef __MACOSX__
typedef void            *HANDLE;
#else
typedef void            *HANDLE;
typedef long           HRESULT;
#endif

typedef int            LRESULT;
typedef unsigned char  byte;
typedef byte *PBYTE;
typedef unsigned short int WORD;
typedef unsigned long long  __u64;
typedef unsigned long long  u64;
typedef unsigned long long  uint64;
typedef __u64 LONGLONG;
typedef long LONG;
typedef long *PLONG;
typedef const void *LPCVOID;
typedef void *LPVOID;
typedef __u64 ULONG64;
typedef int INT;
typedef void *LPARAM;
typedef UINT WPARAM;
typedef void *HWND;
typedef char *PCHAR;
typedef long long __int64;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;

typedef struct _SECURITY_ATTRIBUTES
{
    DWORD nLength;
    /* [size_is] */ LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    BYTE Data4[8];
} GUID;

typedef struct _SECURITY_ATTRIBUTES *PSECURITY_ATTRIBUTES;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;

typedef char CHAR;
typedef struct _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}   SYSTEMTIME;

typedef struct _SYSTEMTIME *PSYSTEMTIME;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _WIN32_FIND_DATA
{
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR   cFileName[ MAX_PATH ];
    CHAR   cAlternateFileName[ 14 ];
#ifdef _MAC
    DWORD dwFileType;
    DWORD dwCreatorType;
    WORD  wFinderFlags;
#endif
} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

#ifndef __MACOSX__
typedef struct _OVERLAPPED
{
    __u64 Internal;
    __u64 InternalHigh;
    union
    {
        struct
        {
            DWORD Offset;
            DWORD OffsetHigh;
        };

        void *Pointer;
    };

    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;
#endif



/*************************************************************/
// Function
/************************************************************/
#include <dlfcn.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//---------------------------------------------------------------//
// max min
//---------------------------------------------------------------//

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

//-----------------------------------------------------------//
//  Library Operator
//----------------------------------------------------------//
#define __declspec(dllexport)

#define GetProcAddress dlsym



HMODULE LoadLibrary(const char *lpFileName)
{
    HMODULE hMod;
    DebugMsg("library file path: %s\n", lpFileName);

    hMod = dlopen(lpFileName, RTLD_LAZY);
    if (hMod == NULL)
        DebugErr("%s\n", dlerror());
    return hMod;
}

int FreeLibrary(HMODULE hModule)
{
    return dlclose(hModule);
}

//---------------------------------------------------------//
//   memory
//---------------------------------------------------------//
static void ZeroMemory(void *Buf, unsigned int nSize)
{
    memset(Buf, 0, nSize);
}

//---------------------------------------------------------//
// ¡Á?¡¤?¡ä?¡ä|¨¤¨ª
//---------------------------------------------------------//
#define _T                                          //?¨ª¡Á?¡¤?
#define _tcschr strchr                              //¡¤¦Ì?????¨°?3¡Á?¡¤?????
#define _tcsstr strstr                              //¡¤¦Ì??¡Á?¡¤?¡ä?????
#define _TUCHAR unsigned char
#define _ttoi atoi                                  //¡Áa??3¨¦int
#define _istdigit isdigit
#define _stprintf sprintf                           //??¨º??¡¥
#define lstrlen _tcslen
#define _tcslen strlen                              //?¨®3¡è?¨¨
#define _vstprintf vsprintf
#define _tcscpy strcpy
#define _tcscmp strcmp
#define _tcscat strcat
#define _getcwd getcwd
#define TCHAR char
#define stricmp strcasecmp                          //o???¡ä¨®D?D¡ä¦Ì?¡À¨¨??

static unsigned int _tclen(const char *_cpc)        //?¨®¡Á?¡¤?3¡è?¨¨
{
    return strlen(_cpc);
}
static char *_tcsinc(const char *current)           //?¨®??¨°???¡Á?¡¤?????
{
    return (char *)current + 1;
}

static char *strlwr(char *str)
{
    char *p = str;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    return str;
}


static char *strupr(char *str)
{
    char *p = str;
    while (*p)
    {
        *p = toupper(*p);
        p++;
    }
    return str;
}

//------------------------------------------------------------//
// File Operator
//-------------------------------------------------------------//
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004
#define FILE_ATTRIBUTE_NORMAL           0x00000080

#define GetCurrentDirectory(nLen, szPath) (getcwd(szPath, nLen))
#define SetCurrentDirectory(szPath)       (chdir(szPath))

static unsigned int GetModuleFileName(void *hInstance, char *szPath, int nLen)  //??¨¨???3¨¬?¡¤??
{
    int nRet;
#ifdef __MACOSX__
    getcwd(szPath, nLen);
#else
    nRet = readlink("/proc/self/exe", szPath, nLen);
    if (nRet < 0 || nRet >= nLen)
    {
        perror("readlink failed:");
        return 0;
    }
#endif
    szPath[nLen - 1] = '\0';

    return nLen;
}

static void GetModuleDirectory(char *szPath)        //??¨¨???3¨¬?¨´?¨²????
{
    char szExePath[MAX_PATH] = {0};
    memset(szExePath, 0, MAX_PATH);
    GetModuleFileName(NULL, szExePath, MAX_PATH);
    strncpy(szPath, szExePath, strlen(szExePath) + 1);
    return;
}


static unsigned int GetFileLength(signed char *filename)
{
    FILE *fp = NULL;
    fp = fopen((const char *)filename, "rb");
    if (fp == NULL)
    {
        return 0;
    }

    long position = 0;
    fseek(fp, 0, SEEK_END);
    unsigned int len = (unsigned int)ftell(fp) - position;
    fclose(fp);
    return len;
}

static DWORD GetLastError()
{
    return errno;
}
// -----------------------------------------------------//
// Others
//------------------------------------------------------//


// -----------------------------------------------------//
// temp file
//------------------------------------------------------//
static unsigned int GetTempPath(unsigned int nBufferLength, char *lpBuffer)
{
    strcpy(lpBuffer, "/tmp");
    return 0;
}

static unsigned int GetTempFileName(const char *lpPathName, const char *lpPrefixString,
                                    unsigned int uUnique, char *lpTempFileName)
{
    strcpy(lpTempFileName, lpPathName);
    strcat(lpTempFileName, "/");
    strcat(lpTempFileName, lpPrefixString);
    strcat(lpTempFileName, ".tmp");

    creat(lpTempFileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    return 0;
}

static  BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
    int y, m, d, h, minute, sec;

    WORD tem;

    WORD date;
    WORD dtm;


    y = lpSystemTime->wYear - 1980;

    m = lpSystemTime->wMonth;
    d = lpSystemTime->wDay;
    h = lpSystemTime->wHour;
    minute = lpSystemTime->wMinute;
    sec = lpSystemTime->wSecond;

    date = d;
    tem = m;
    tem = tem << 5;
    date = date | tem;

    tem = y;
    tem = tem << 9;
    date = date | tem;

    dtm = sec;
    tem = minute;
    tem = tem << 5;
    dtm = dtm | tem;

    tem = h;
    tem  = tem << 11;
    dtm = dtm | tem;

    memcpy(&lpFileTime->dwHighDateTime, &date, 2);
    memcpy(&lpFileTime->dwLowDateTime, &dtm, 2);

    return TRUE;
}

static BOOL LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime)
{
    memcpy(lpFileTime, lpLocalFileTime, sizeof(FILETIME));
    return TRUE;
}

static int MultiByteToWideChar(UINT CodePage,
                               DWORD dwFlags,
                               LPCSTR lpMultiByteStr,
                               int cbMultiByte,
                               LPWSTR lpWideCharStr,
                               int cchWideChar
                              )
{
    return mbstowcs(lpWideCharStr, lpMultiByteStr, cchWideChar);
}

static int WideCharToMultiByte(INT CodePage,
                               DWORD dwFlags,
                               LPCWSTR lpWideCharStr,
                               int cchWideChar,
                               LPSTR lpMultiByteStr,
                               int cbMultiByte,
                               LPCSTR lpDefaultChar,
                               bool *lpUsedDefaultChar
                              )
{
    return wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte);
}


/************************************************************/
// CreateThread
typedef void *(*pTHREAD_START_ROUTINE)(void *pParam);
static int  CreateThread(
    void  *lpThreadAttributes,
    int  dwStackSize,
    pTHREAD_START_ROUTINE lpStartAddress,
    void *lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId)
{
    pthread_t tid;
    return pthread_create(&tid, NULL, lpStartAddress, (void *) lpParameter);
}


//
/***********************************************************/
//  Class
/**********************************************************/
#include "BCString.h"
typedef CBCString CString;

#endif //_WINDOWS

/****************************************************************/

#endif //_X_COMPLIER_DEF_H__HEADER_
