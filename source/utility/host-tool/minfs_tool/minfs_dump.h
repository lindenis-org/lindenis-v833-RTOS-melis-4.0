/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_dump.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-25
* Descript: Mini rom image dump tool.
* Update  : date                auther      ver     notes
*           2011-3-25 19:18:19  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MINFS_DUMP_H__
#define __MINFS_DUMP_H__

#define MINFS_DEFAULT_SECTION_NAME  "MELIS_MOD"

typedef struct __DUMP_CONTEXT
{
    __u8    StoreDir[MAX_PATH];
    __u8   *pBuffer;
    __u32   BufferLen;
    __u32   DirNum;
    __u32   FileNum;
} __dump_context_t;

__s32 MINFS_DumpImage(const char *pDstDir, const char *pSrcFile);

#endif  //__MINFS_DUMP_H__
