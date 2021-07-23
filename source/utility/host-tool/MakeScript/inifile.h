#ifndef _INI_FILE_H
#define _INI_FILE_H

#include <string.h>

#ifndef UINT
typedef unsigned int UINT;
#endif
#ifndef LPCSTR
typedef const char *LPCSTR;
#endif
#ifndef LPSTR
typedef char *LPSTR;
#endif
#ifndef INT
typedef int INT;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

UINT
GetPrivateProfileInt(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    INT nDefault,
    LPCSTR lpFileName
);
BOOL
WritePrivateProfileString(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpString,
    LPCSTR lpFileName
);
DWORD
GetPrivateProfileString(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpDefault,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
);

DWORD
GetPrivateProfileSection(
    LPCSTR lpAppName,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
);

DWORD
GetPrivateProfileSectionNames(
    LPSTR lpszReturnBuffer,
    DWORD nSize,
    LPCSTR lpFileName
);

BOOL
WritePrivateProfileSection(
    LPCSTR lpAppName,
    LPCSTR lpString,
    LPCSTR lpFileName
);

#endif /* __CONFIG_H__ */
