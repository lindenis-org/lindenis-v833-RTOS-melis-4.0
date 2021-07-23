/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_types.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-25
* Descript: Mini rom image file system data types defines.
* Update  : date                auther      ver     notes
*           2011-3-25 13:45:59  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

#define OK         0
#define FAIL       (-1)

typedef unsigned int    __u32;
typedef unsigned short  __u16;
typedef unsigned char   __u8;
typedef signed int      __s32;
typedef signed short    __s16;
typedef signed char     __s8;
typedef signed char     __bool;

typedef unsigned int    u32;
typedef unsigned short  u16;
typedef unsigned char   u8;
typedef signed int      s32;
typedef signed short    s16;
typedef signed char     s8;
typedef signed char     bool;

typedef char TCHAR;
typedef u32 DWORD;


#ifndef DWORD
typedef u32 DWORD;
#endif

#define MSG                     printf
#define Msg                     printf

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

/*
 * #ifndef DWORD
typedef unsigned long DWORD;
#endif
*/
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#endif  //__MINFS_TYPES_H__
