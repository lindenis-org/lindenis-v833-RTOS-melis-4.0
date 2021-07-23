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

#ifndef __MINFS_TYPES_H__
#define __MINFS_TYPES_H__

#define EPDK_OK         0
#define EPDK_FAIL       (-1)

typedef unsigned int    __u32;
typedef unsigned short  __u16;
typedef unsigned char   __u8;
typedef signed int      __s32;
typedef signed short    __s16;
typedef signed char     __s8;
typedef signed char     __bool;

#define MSG                     printf
#define MINFS_ALIGN(val, align) (((val) + ((align) - 1)) & ~((align) - 1))

#endif  //__MINFS_TYPES_H__
